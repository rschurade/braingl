/*
 * fiberrenderer.cpp
 *
 *  Created on: 28.12.2012
 *      Author: Ralph Schurade
 */
#include "fiberrenderer.h"
#include "fiberrendererthread.h"

#include "glfunctions.h"

#include "../../data/enums.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>

FiberRenderer::FiberRenderer( QAbstractItemModel* roiModel, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData )  :
    ObjectRenderer(),
    m_roiModel( roiModel ),
    vboIds( new GLuint[ 2 ] ),
    m_data( data ),
    m_extraData( extraData ),
    m_numLines( data.size() ),
    m_numPoints( 0 ),
    m_isInitialized( false ),
    m_colorMode( 0 ),
    m_colormap( 1 ),
    m_selectedMin( 0.0 ),
    m_selectedMax( 1.0 ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 ),
    m_kdTree( 0 )
{
    m_boxMin.resize( 3 );
    m_boxMax.resize( 3 );

    connect( m_roiModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( roiChanged( QModelIndex, QModelIndex ) ) );
    connect( m_roiModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( roiInserted( QModelIndex, int, int ) ) );
    connect( m_roiModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( roiDeleted( QModelIndex, int, int ) ) );

    m_rootfield.resize( m_numLines );
    for ( int i = 0; i < m_numLines; ++i )
    {
        m_rootfield[i] = true;
    }
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

QVector<bool> FiberRenderer::getSelection()
{
    return m_rootfield;
}

void FiberRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert, QAbstractItemModel* dataModel )
{
    GLFunctions::getShader( "fiber" )->bind();

    GLFunctions::setupTextures( dataModel );
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "fiber" ) , dataModel );

    // Set modelview-projection matrix
    GLFunctions::getShader( "fiber" )->setUniformValue( "mvp_matrix", mvp_matrix );
    GLFunctions::getShader( "fiber" )->setUniformValue( "mv_matrixInvert", mv_matrixInvert );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_colorMode", m_colorMode );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_colormap", m_colormap );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_color", 1.0, 0.0, 0.0 );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_selectedMin", m_selectedMin );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_selectedMax", m_selectedMax );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_lowerThreshold", m_lowerThreshold );
    GLFunctions::getShader( "fiber" )->setUniformValue( "u_upperThreshold", m_upperThreshold );

    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    if ( m_colorMode != 2 )
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( m_rootfield[i] )
            {
                glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( m_rootfield[i] )
            {
                GLFunctions::getShader( "fiber" )->setUniformValue( "u_color", m_colorField[i].redF(), m_colorField[i].greenF(), m_colorField[i].blueF() );
                glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
            }
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FiberRenderer::setupTextures()
{
}

void FiberRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "fiber" );

    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int colorLocation = program->attributeLocation( "a_color" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int extraLocation = program->attributeLocation( "a_extra" );
    program->enableAttributeArray( extraLocation );
    glVertexAttribPointer( extraLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );
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
        threads.push_back( new FiberRendererThread( m_data, m_extraData, i ) );
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

    qDebug() << "start creating kdtree";
    m_numPoints = verts.size() / 10;
    m_kdVerts.reserve( m_numPoints * 3 );
    m_reverseIndexes.reserve( m_numPoints );

    m_colorField.reserve( m_numLines );
    QColor color( 255, 0, 0 );
    for ( int i = 0; i < m_numLines; ++i )
    {
        for( int k = 0; k < m_data[i].size() / 3; ++k )
        {
            m_kdVerts.push_back( m_data[i][k * 3    ] );
            m_kdVerts.push_back( m_data[i][k * 3 + 1] );
            m_kdVerts.push_back( m_data[i][k * 3 + 2] );
            m_reverseIndexes.push_back( i );
        }
        m_colorField.push_back( color );
    }
    m_kdTree = new KdTree( m_numPoints, m_kdVerts.data() );
    qDebug() << "end creating kdtree";

    updatePresentRois();

    m_isInitialized = true;
}

void FiberRenderer::setRenderParams( int colorMode, int colormap, float selectedMin, float selectedMax, float lowerThreshold, float upperThreshold )
{
    m_colorMode = colorMode;
    m_colormap = colormap;
    m_selectedMin = selectedMin;
    m_selectedMax = selectedMax;
    m_lowerThreshold = lowerThreshold;
    m_upperThreshold = upperThreshold;
}

void FiberRenderer::updatePresentRois()
{
    int numBranches = m_roiModel->rowCount( QModelIndex() );

    for ( int i = 0; i < numBranches; ++i )
    {
        QList<QVector<bool> >newBranch;
        QVector<bool>newLeaf( m_numLines );
        newBranch.push_back( newLeaf );
        m_branchfields.push_back( newLeaf );
        m_bitfields.push_back( newBranch );
        updateBox( m_bitfields.size() - 1, 0 );

        int leafCount = m_roiModel->rowCount( createIndex( i, 0, 0 ) );

        for ( int k = 0; k < leafCount; ++k )
        {
            // inserted child roi
            QVector<bool>newLeaf( m_numLines );
            m_bitfields[i].push_back( newLeaf );
            updateBox( i, k + 1 );
        }
    }
}

void FiberRenderer::roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    if ( topLeft.row() == -1 ) return;
    //qDebug() << "roi changed" << topLeft.row() << topLeft.column() << topLeft.internalId();
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
    updateBox( branch, pos );

}

void FiberRenderer::roiInserted( const QModelIndex &parent, int start, int end )
{
    //qDebug() << "roi inserted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        // inserted top level roi
        QList<QVector<bool> >newBranch;
        QVector<bool>newLeaf( m_numLines );
        newBranch.push_back( newLeaf );
        m_branchfields.push_back( newLeaf );
        m_bitfields.push_back( newBranch );
        updateBox( m_bitfields.size() - 1, 0 );
    }
    else
    {
        // inserted child roi
        QVector<bool>newLeaf( m_numLines );
        m_bitfields[parent.row()].push_back( newLeaf );
        updateBox( parent.row(), m_bitfields[parent.row()].size() - 1 );
    }
}

void FiberRenderer::roiDeleted( const QModelIndex &parent, int start, int end )
{
    //qDebug() << "roi deleted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        m_bitfields.removeAt( start );
        m_branchfields.removeAt( start );
        updateRoot();
    }
    else
    {
        m_bitfields[parent.row()].removeAt( start + 1 );
        updateBranch( parent.row() );
    }
}

QModelIndex FiberRenderer::createIndex( int branch, int pos, int column )
{
    int row;
    QModelIndex parent;
    if ( pos == 0 )
    {
        row = branch;
    }
    else
    {
        row = pos - 1;
        parent = m_roiModel->index( branch, 0 );
    }
    return m_roiModel->index( row, column, parent );
}

void FiberRenderer::updateBox( int branch, int pos )
{
    //qDebug() << "update box" << branch << pos;
    float x, y, z, dx, dy, dz;

    if ( m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
    {
        x = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::X ), Qt::DisplayRole ).toFloat();
        y = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::Y ), Qt::DisplayRole ).toFloat();
        z = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::Z ), Qt::DisplayRole ).toFloat();
        dx = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::DX ), Qt::DisplayRole ).toFloat();
        dy = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::DY ), Qt::DisplayRole ).toFloat();
        dz = m_roiModel->data( createIndex( branch, pos, (int)Fn::ROI::DZ ), Qt::DisplayRole ).toFloat();
        m_boxMin[0] = x - dx / 2;
        m_boxMax[0] = x + dx / 2;
        m_boxMin[1] = y - dy / 2;
        m_boxMax[1] = y + dy / 2;
        m_boxMin[2] = z - dz / 2;
        m_boxMax[2] = z + dz / 2;

        for ( int i = 0; i < m_numLines; ++i )
        {
            m_bitfields[branch][pos][i] = false;
        }
        boxTest( m_bitfields[branch][pos], 0, m_numPoints - 1, 0 );
    }
    else
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_bitfields[branch][pos][i] = true;
        }
    }

    updateBranch( branch );
    if ( m_isInitialized )
    {
        m_roiModel->setData( createIndex( branch, pos, (int)Fn::ROI::UPDATED ), true, Qt::DisplayRole );
    }
}

void FiberRenderer::boxTest( QVector<bool>& workfield, int left, int right, int axis )
{
    // abort condition
    if ( left > right )
        return;

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if ( m_kdVerts[pointIndex + axis] < m_boxMin[axis] )
    {
        boxTest( workfield, root + 1, right, axis1 );
    }
    else if ( m_kdVerts[pointIndex + axis] > m_boxMax[axis] )
    {
        boxTest( workfield, left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if ( m_kdVerts[pointIndex + axis1] <= m_boxMax[axis1] && m_kdVerts[pointIndex + axis1]
                >= m_boxMin[axis1] && m_kdVerts[pointIndex + axis2] <= m_boxMax[axis2]
                && m_kdVerts[pointIndex + axis2] >= m_boxMin[axis2] )
        {
            workfield[m_reverseIndexes[ m_kdTree->m_tree[root] ]] = true;
        }
        boxTest( workfield, left, root - 1, axis1 );
        boxTest( workfield, root + 1, right, axis1 );
    }
}


void FiberRenderer::updateBranch( int branch )
{
    //qDebug() << "update branch" << branch;
    int current = 0;

    bool neg = m_roiModel->data( createIndex( branch, current, (int)Fn::ROI::NEG ), Qt::DisplayRole ).toBool();
    if ( neg )
    {
        for ( int k = 0; k < m_numLines; ++k )
        {
            m_branchfields[branch][k] = !m_bitfields[branch][current][k];
        }
    }
    else
    {
        for ( int k = 0; k < m_numLines; ++k )
        {
            m_branchfields[branch][k] = m_bitfields[branch][current][k];
        }
    }
    ++current;

    while ( current < m_bitfields[branch].size() )
    {
        if ( m_roiModel->data( createIndex( branch, current, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
        {
            bool neg = m_roiModel->data( createIndex( branch, current, (int)Fn::ROI::NEG ), Qt::DisplayRole ).toBool();
            if ( neg )
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_branchfields[branch][k] &= !m_bitfields[branch][current][k];
                }
            }
            else
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_branchfields[branch][k] &= m_bitfields[branch][current][k];
                }
            }
            ++current;
        }
        else
        {
            ++current;
        }
    }
    updateRoot();
}

void FiberRenderer::updateRoot()
{
    //qDebug() << "update root";
    if ( m_branchfields.size() > 0 )
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_rootfield[i] = false;
        }

        for ( int i = 0; i < m_branchfields.size(); ++i )
        {
            if ( m_roiModel->data( createIndex( i, 0, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_rootfield[k] |= m_branchfields[i][k];
                }
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_rootfield[i] = true;
        }
    }
}

void FiberRenderer::colorChanged( QColor color )
{
    for ( int i = 0; i < m_numLines; ++i )
    {
        if ( m_rootfield[i] )
        {
            m_colorField.replace( i, color );
        }
    }
}


