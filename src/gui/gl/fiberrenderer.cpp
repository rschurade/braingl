/*
 * fiberrenderer.cpp
 *
 *  Created on: 28.12.2012
 *      Author: Ralph
 */
#include "fiberrenderer.h"
#include "fiberrendererthread.h"

#include "glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>

FiberRenderer::FiberRenderer( QAbstractItemModel* roiModel, QVector< QVector< float > >& data )  :
    ObjectRenderer(),
    m_roiModel( roiModel ),
    vboIds( new GLuint[ 2 ] ),
    m_data( data ),
    m_isInitialized( false ),
    m_colorMode( 0 )
{
    connect( m_roiModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( roiChanged( QModelIndex, QModelIndex ) ) );
    connect( m_roiModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( roiInserted( QModelIndex, int, int ) ) );
    connect( m_roiModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( roiDeleted( QModelIndex, int, int ) ) );

    m_bitfield.resize( data.size() );
}

FiberRenderer::~FiberRenderer()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
    glDeleteBuffers(1, &( vboIds[ 1 ] ) );
}

void FiberRenderer::init()
{
    glGenBuffers( 2, vboIds );
}

void FiberRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
{
    GLFunctions::getShader( "fiber" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "fiber" )->setUniformValue( "mvp_matrix", mvp_matrix );
    GLFunctions::getShader( "fiber" )->setUniformValue( "mv_matrixInvert", mv_matrixInvert );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_colorMode", m_colorMode );

    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    for ( int i = 0; i < m_data.size(); ++i )
    {
        glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FiberRenderer::setupTextures()
{
}

void FiberRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "fiber", model() );
}

void FiberRenderer::initGeometry()
{
    if ( m_isInitialized )
    {
        return;
    }
    qDebug() << "create fiber vbo's...";
    int numThreads = QThread::idealThreadCount();

    QVector<FiberRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new FiberRendererThread( m_data, i ) );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->wait();
    }

    QVector<float> verts;
    // combine verts from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        verts += *( threads[i]->getVerts() );
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );

    m_pointsPerLine.resize( m_data.size() );
    m_startIndexes.resize( m_data.size() );

    int currentStart = 0;
    for ( int i = 0; i < m_data.size(); ++i )
    {
        m_pointsPerLine[i] = m_data[i].size() / 3;
        m_startIndexes[i] = currentStart;
        currentStart += m_pointsPerLine[i];
    }
    qDebug() << "create fiber vbo's done";

    QVector<float>kdVerts;
    kdVerts.reserve( verts.size() / 9 * 3 );
    m_reverseIndexes.reserve( verts.size() / 9 );
    for ( int i = 0; i < m_data.size(); ++i )
    {
        for( int k = 0; k < m_data[i].size() / 3; ++k )
        {
            kdVerts.push_back( m_data[i][k * 3    ] );
            kdVerts.push_back( m_data[i][k * 3 + 1] );
            kdVerts.push_back( m_data[i][k * 3 + 2] );
            m_reverseIndexes.push_back( i );
        }
    }
    qDebug() << "start creating kdtree";
    m_kdTree = new KdTree( kdVerts.size() / 3, kdVerts.data() );
    qDebug() << "end creating kdtree";

    m_isInitialized = true;
}

void FiberRenderer::setRenderParams( int colorMode )
{
    m_colorMode = colorMode;
}

void FiberRenderer::roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    qDebug() << "roi changed" << topLeft.row() << topLeft.column() << topLeft.internalId();
    int branch = 0;
    int pos = 0;
    if ( topLeft.internalId() == -1 )
    {
        // top level box
        branch = topLeft.row();
    }
    else
    {
        // child box
        branch = topLeft.internalId();
        pos = topLeft.row() + 1;
    }

}

void FiberRenderer::roiInserted( const QModelIndex &parent, int start, int end )
{
    qDebug() << "roi inserted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        // inserted top level roi
        QList<QVector<bool> >newBranch;
        QVector<bool>newLeaf( m_data.size() );
        newBranch.push_back( newLeaf );
        m_branchfields.push_back( newLeaf );
        m_bitfields.push_back( newBranch );
    }
    else
    {
        // inserted child roi
        QVector<bool>newLeaf( m_data.size() );
        m_bitfields[parent.row()].push_back( newLeaf );
    }
}

void FiberRenderer::roiDeleted( const QModelIndex &parent, int start, int end )
{
    qDebug() << "roi deleted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        m_bitfields.removeAt( start );
        m_branchfields.removeAt( start );
    }
    else
    {
        m_bitfields[parent.row()].removeAt( start + 1 );
    }
}
