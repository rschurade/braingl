/*
 * colormaprenderer.cpp
 *
 *  Created on: Feb 18, 2013
 *      Author: schurade
 */
#include "colormaprenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtOpenGL/QGLShaderProgram>

ColormapRenderer::ColormapRenderer() :
    vboIds( new GLuint[ 3 ] ),
    m_orient( 0 ),
    m_x( 0.0f ),
    m_y( 0.0f ),
    m_dx( 1.0f ),
    m_dy( 1.0f ),
    m_colormap( 0 ),
    m_min( 0.0 ),
    m_max( 1.0 ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 )
{
}

ColormapRenderer::~ColormapRenderer()
{
}

void ColormapRenderer::init()
{
    GLFunctions::loadShaders();
    glGenBuffers( 3, vboIds );

    initGeometry();
}


void ColormapRenderer::initGeometry()
{
    GLushort indices[] = { 0, 1, 2, 3 };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLushort), indices, GL_STATIC_DRAW );
    float z = -0.5f;
    VertexData vertices[] =
    {
        { QVector3D( m_x,        m_y - m_dy, z ), QVector3D( 0.0, 0.0, 0.0 ) },
        { QVector3D( m_x + m_dx, m_y - m_dy, z ), QVector3D( 1.0, 0.0, 0.0 ) },
        { QVector3D( m_x + m_dx, m_y,        z ), QVector3D( 1.0, 1.0, 0.0 ) },
        { QVector3D( m_x,        m_y,        z ), QVector3D( 0.0, 1.0, 0.0 ) }
    };

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

    z = -0.51f;
    float lineWidth = 0.005;

    float value = m_selectedMin / m_max;
    value = ( value - m_lowerThreshold/m_max )  / ( m_upperThreshold/m_max - m_lowerThreshold/m_max  );
    value = qMax( 0.0f, qMin( 1.0f, value ) );

    float value1 = 0.25 * (  m_selectedMax / m_max - m_selectedMin / m_max ) + m_selectedMin / m_max;
    value1 = ( value1 - m_lowerThreshold/m_max )  / ( m_upperThreshold/m_max - m_lowerThreshold/m_max  );
    value1 = qMax( 0.0f, qMin( 1.0f, value1 ) );

    float value2 = 0.5 * (  m_selectedMax / m_max - m_selectedMin / m_max ) + m_selectedMin / m_max;
    value2 = ( value2 - m_lowerThreshold/m_max )  / ( m_upperThreshold/m_max - m_lowerThreshold/m_max  );
    value2 = qMax( 0.0f, qMin( 1.0f, value2 ) );

    float value3 = 0.75 * (  m_selectedMax / m_max - m_selectedMin / m_max ) + m_selectedMin / m_max;
    value3 = ( value3 - m_lowerThreshold/m_max )  / ( m_upperThreshold/m_max - m_lowerThreshold/m_max  );
    value3 = qMax( 0.0f, qMin( 1.0f, value3 ) );

    float value4 = 1.0 * (  m_selectedMax / m_max - m_selectedMin / m_max ) + m_selectedMin / m_max;
    value4 = ( value4 - m_lowerThreshold/m_max )  / ( m_upperThreshold/m_max - m_lowerThreshold/m_max  );
    value4 = qMax( 0.0f, qMin( 1.0f, value4 ) );

    float scaleVertices[] =
    {
        m_x,        m_y - m_dy + lineWidth, z,
        m_x + m_dx, m_y - m_dy + lineWidth, z,
        m_x + m_dx, m_y - m_dy,             z,
        m_x,        m_y - m_dy,             z,

        m_x + value * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + value * m_dx + lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + value * m_dx + lineWidth, m_y - m_dy,               z,
        m_x + value * m_dx,             m_y - m_dy,               z,

        m_x + value1 * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + value1 * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + value1 * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + value1 * m_dx,             m_y - m_dy,               z,

        m_x + value2 * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + value2 * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + value2 * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + value2 * m_dx,             m_y - m_dy,               z,

        m_x + value3 * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + value3 * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + value3 * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + value3 * m_dx,             m_y - m_dy,               z,

        m_x + value4 * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + value4 * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + value4 * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + value4 * m_dx,             m_y - m_dy,               z
    };
    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ARRAY_BUFFER, 72 * sizeof(float), scaleVertices, GL_STATIC_DRAW );
}

void ColormapRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "colormapbar" );
    program->bind();
    // Offset for position
    long int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation( "a_texcoord" );
    program->enableAttributeArray( texcoordLocation );
    glVertexAttribPointer( texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    program->setUniformValue( "u_colormap", m_colormap );
    program->setUniformValue( "u_min", m_selectedMin / m_max );
    program->setUniformValue( "u_max", m_selectedMax / m_max );
    program->setUniformValue( "u_lowerThreshold", m_lowerThreshold / m_max );
    program->setUniformValue( "u_upperThreshold", m_upperThreshold / m_max );
}

void ColormapRenderer::draw()
{
    GLFunctions::getShader( "colormapbar" )->bind();
    // Set modelview-projection matrix

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_QUADS, 4, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );

    QGLShaderProgram* program = GLFunctions::getShader( "colormapscale" );
    program->bind();
    // Offset for position
    long int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (const void *) offset );

    glDrawArrays( GL_QUADS, 0, 24 ); // third argument is count verts in buffer, not count quads

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void ColormapRenderer::setX( float x )
{
    m_x = 2* x - 1.0;
    initGeometry();
}
void ColormapRenderer::setY( float y )
{
    m_y = -2 * y + 1.0;
    initGeometry();
}
void ColormapRenderer::setDX( float dx )
{
    m_dx = 2* dx;
    initGeometry();
}
void ColormapRenderer::setDY( float dy )
{
    m_dy = 2 * dy;
    initGeometry();
}

void ColormapRenderer::setColormap( int value )
{
    m_colormap = value;
}
void ColormapRenderer::setMin( float value )
{
    m_min = value;
}
void ColormapRenderer::setMax( float value )
{
    m_max = value;
}
void ColormapRenderer::setSelectedMin( float value )
{
    m_selectedMin = value;
}
void ColormapRenderer::setSelectedMax( float value )
{
    m_selectedMax = value;
}

void ColormapRenderer::setLowerThreshold( float value )
{
    m_lowerThreshold = value;
}
void ColormapRenderer::setUpperThreshold( float value )
{
    m_upperThreshold = value;
}
