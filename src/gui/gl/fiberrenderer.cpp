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
#include "../../data/datasets/fiberselector.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

FiberRenderer::FiberRenderer( QAbstractItemModel* roiModel, FiberSelector* selector, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData )  :
    ObjectRenderer(),
    m_roiModel( roiModel ),
    m_selector( selector ),
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
    m_upperThreshold( 1.0 )
{
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

void FiberRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, QAbstractItemModel* dataModel )
{
    GLFunctions::getShader( "fiber" )->bind();

    GLFunctions::setupTextures( dataModel );
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "fiber" ) , dataModel );

    // Set modelview-projection matrix
    GLFunctions::getShader( "fiber" )->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    GLFunctions::getShader( "fiber" )->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );
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

    QVector<bool>*selected = m_selector->getSelection();

    if ( m_colorMode != 2 )
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( selected->at( i ) )
            {
                glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( selected->at( i ) )
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
    int numThreads = GLFunctions::idealThreadCount;

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

    m_numPoints = verts.size() / 10;

    m_colorField.reserve( m_numLines );
    QColor color( 255, 0, 0 );
    for ( int i = 0; i < m_numLines; ++i )
    {
        m_colorField.push_back( color );
    }

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

void FiberRenderer::colorChanged( QColor color )
{
    QVector<bool>*selected = m_selector->getSelection();
    for ( int i = 0; i < m_numLines; ++i )
    {
        if ( selected->at( i ) )
        {
            m_colorField.replace( i, color );
        }
    }
}


