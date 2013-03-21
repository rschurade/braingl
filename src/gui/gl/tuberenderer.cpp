/*
 * fiberrenderer.cpp
 *
 *  Created on: 28.12.2012
 *      Author: Ralph Schurade
 */
#include "tuberenderer.h"
#include "tuberendererthread.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/datasets/fiberselector.h"
#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

TubeRenderer::TubeRenderer( FiberSelector* selector, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData )  :
    ObjectRenderer(),
    m_selector( selector ),
    vboIds( new GLuint[ 2 ] ),
    m_data( data ),
    m_extraData( extraData ),
    m_numLines( data.size() ),
    m_numPoints( 0 ),
    m_isInitialized( false )
{
}

TubeRenderer::~TubeRenderer()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
    glDeleteBuffers(1, &( vboIds[ 1 ] ) );
}

void TubeRenderer::init()
{
    glGenBuffers( 2, vboIds );
}

void TubeRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, PropertyGroup* props )
{
    GLFunctions::getShader( "tube" )->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "tube" ) );

    // Set modelview-projection matrix
    GLFunctions::getShader( "tube" )->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    GLFunctions::getShader( "tube" )->setUniformValue( "mv_matrixTI", mv_matrix.transposed().inverted() );


    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars( props );

    QVector<bool>*selected = m_selector->getSelection();
    if ( props->get( Fn::Property::COLORMODE ).toInt() != 2 )
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( selected->at( i ) )
            {
                glDrawArrays( GL_QUAD_STRIP, m_startIndexes[i]*2, m_pointsPerLine[i]*2 );
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_data.size(); ++i )
        {
            if ( selected->at( i ) )
            {
                GLFunctions::getShader( "tube" )->setUniformValue( "u_color", m_colorField[i].redF(), m_colorField[i].greenF(), m_colorField[i].blueF() );
                glDrawArrays( GL_QUAD_STRIP, m_startIndexes[i]*2, m_pointsPerLine[i]*2 );
            }
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void TubeRenderer::setupTextures()
{
}

void TubeRenderer::setShaderVars( PropertyGroup* props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "tube" );

    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (const void *) offset );

    offset += sizeof(float) * 3;
    int colorLocation = program->attributeLocation( "a_color" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (const void *) offset );

    offset += sizeof(float) * 3;
    int extraLocation = program->attributeLocation( "a_extra" );
    program->enableAttributeArray( extraLocation );
    glVertexAttribPointer( extraLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (const void *) offset );

    offset += sizeof(float);
    int dirLocation = program->attributeLocation( "a_direction" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (const void *) offset );

    GLFunctions::getShader( "tube" )->setUniformValue( "u_colorMode", props->get( Fn::Property::COLORMODE ).toInt() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_colormap", props->get( Fn::Property::COLORMAP ).toInt() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_color", 1.0, 0.0, 0.0, 1.0 );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_selectedMin", props->get( Fn::Property::SELECTED_MIN ).toFloat() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_selectedMax", props->get( Fn::Property::SELECTED_MAX ).toFloat() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_lowerThreshold", props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_upperThreshold", props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
    GLFunctions::getShader( "tube" )->setUniformValue( "u_thickness", props->get( Fn::Property::FIBER_THICKNESS ).toFloat() / 100.f );
}

void TubeRenderer::initGeometry()
{
    if ( m_isInitialized )
    {
        return;
    }
    qDebug() << "create fiber vbo's...";
    int numThreads = GLFunctions::idealThreadCount;

    QVector<TubeRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new TubeRendererThread( m_data, m_extraData, i ) );
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

void TubeRenderer::colorChanged( QColor color )
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


