/*
 * fiberrenderer.cpp
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */
#include "fiberrenderer.h"
#include "fiberrendererthread.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/datasets/fiberselector.h"

#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

FiberRenderer::FiberRenderer( FiberSelector* selector, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData )  :
    ObjectRenderer(),
    m_selector( selector ),
    vboIds( new GLuint[ 2 ] ),
    m_data( data ),
    m_extraData( extraData ),
    m_numLines( data.size() ),
    m_numPoints( 0 ),
    m_isInitialized( false )
{
    m_colorField.resize( m_numLines );
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

void FiberRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    float alpha = props->get( Fn::Property::ALPHA ).toFloat();
    if ( renderMode == 1 ) // we are drawing opaque objects
    {
        if ( alpha < 1.0 )
        {
            // obviously not opaque
            return;
        }
    }
    else // we are drawing tranparent objects
    {
        if ( !(alpha < 1.0 ) )
        {
            // not transparent
            return;
        }
    }

    QGLShaderProgram* program = GLFunctions::getShader( "fiber" );
    program->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "fiber" ) );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars( props );

    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    glLineWidth( props->get( Fn::Property::FIBER_THICKNESS ).toFloat() );

    QVector<bool>*selected = m_selector->getSelection();

    if ( props->get( Fn::Property::COLORMODE ).toInt() != 2 )
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
                program->setUniformValue( "u_color", m_colorField[i].redF(),
                                                                                 m_colorField[i].greenF(),
                                                                                 m_colorField[i].blueF(), 1.0 );
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

void FiberRenderer::setShaderVars( PropertyGroup* props )
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

    program->setUniformValue( "u_colorMode", props->get( Fn::Property::COLORMODE ).toInt() );
    program->setUniformValue( "u_colormap", props->get( Fn::Property::COLORMAP ).toInt() );
    program->setUniformValue( "u_color", 1.0, 0.0, 0.0, 1.0 );
    program->setUniformValue( "u_selectedMin", props->get( Fn::Property::SELECTED_MIN ).toFloat() );
    program->setUniformValue( "u_selectedMax", props->get( Fn::Property::SELECTED_MAX ).toFloat() );
    program->setUniformValue( "u_lowerThreshold", props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_upperThreshold", props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_dx", props->get( Fn::Property::DX ).toFloat() );
    program->setUniformValue( "u_dy", props->get( Fn::Property::DY ).toFloat() );
    program->setUniformValue( "u_dz", props->get( Fn::Property::DZ ).toFloat() );
    program->setUniformValue( "u_x", props->get( Fn::Property::NX ).toFloat() / 10.f );
    program->setUniformValue( "u_y", props->get( Fn::Property::NY ).toFloat() / 10.f );
    program->setUniformValue( "u_z", props->get( Fn::Property::NZ ).toFloat() / 10.f );
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

    m_isInitialized = true;
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


