/*
 * colormaprenderer.cpp
 *
 * Created on: Feb 18, 2013
 * @author schurade
 */
#include "colormaprenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtOpenGL/QGLShaderProgram>

ColormapRenderer::ColormapRenderer() :
    vboIds( new GLuint[ 2 ] ),
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
    glDeleteBuffers( 2, &( vboIds[ 0 ] ) );
}

void ColormapRenderer::init()
{
    glGenBuffers( 2, vboIds );

    initGeometry();
}


void ColormapRenderer::initGeometry()
{
    float z = -0.5f;
    VertexData vertices[] =
    {
        { QVector3D( m_x,        m_y - m_dy, z ), QVector3D( 0.0, 0.0, 0.0 ) },
        { QVector3D( m_x + m_dx, m_y - m_dy, z ), QVector3D( 1.0, 0.0, 0.0 ) },
        { QVector3D( m_x + m_dx, m_y,        z ), QVector3D( 1.0, 1.0, 0.0 ) },
        { QVector3D( m_x,        m_y,        z ), QVector3D( 0.0, 1.0, 0.0 ) }
    };

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

    z = -0.51f;
    float lineWidth = 2;

    m_labels.clear();

    float value = m_selectedMin;
    m_labels.push_back( QVector3D( m_x, m_y, value ) );

    float value1 = 0.25 * (  m_selectedMax - m_selectedMin ) + m_selectedMin;
    m_labels.push_back( QVector3D( m_x + 0.25 * m_dx, m_y, value1 ) );

    float value2 = 0.5 * (  m_selectedMax - m_selectedMin ) + m_selectedMin;
    m_labels.push_back( QVector3D( m_x + 0.5 * m_dx, m_y, value2 ) );

    float value3 = 0.75 * (  m_selectedMax - m_selectedMin ) + m_selectedMin;
    m_labels.push_back( QVector3D( m_x + 0.75 * m_dx, m_y, value3 ) );

    float value4 = 1.0 * (  m_selectedMax - m_selectedMin ) + m_selectedMin;
    m_labels.push_back( QVector3D( m_x + 1.0 * m_dx, m_y, value4 ) );



    float scaleVertices[] =
    {
        m_x,        m_y - m_dy + lineWidth, z,
        m_x + m_dx, m_y - m_dy + lineWidth, z,
        m_x + m_dx, m_y - m_dy,             z,
        m_x,        m_y - m_dy,             z,

        m_x,             m_y - m_dy + lineWidth*3, z,
        m_x + lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + lineWidth, m_y - m_dy,               z,
        m_x,             m_y - m_dy,               z,

        m_x + 0.25f * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + 0.25f * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + 0.25f * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + 0.25f * m_dx,             m_y - m_dy,               z,

        m_x + 0.5f * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + 0.5f * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + 0.5f * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + 0.5f * m_dx,             m_y - m_dy,               z,

        m_x + 0.75f * m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + 0.75f * m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + 0.75f * m_dx - lineWidth, m_y - m_dy,               z,
        m_x + 0.75f * m_dx,             m_y - m_dy,               z,

        m_x + m_dx,             m_y - m_dy + lineWidth*3, z,
        m_x + m_dx - lineWidth, m_y - m_dy + lineWidth*3, z,
        m_x + m_dx - lineWidth, m_y - m_dy,               z,
        m_x + m_dx,             m_y - m_dy,               z
    };
    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 72 * sizeof(float), scaleVertices, GL_STATIC_DRAW );
}

void ColormapRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "colormapbar" );
    program->bind();

    // Offset for position
    intptr_t offset = 0;

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

void ColormapRenderer::draw( int width, int height, int renderMode )
{
    if ( renderMode == 1 ) // we are drawing opaque objects
    {
        return;
    }

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    setShaderVars();

    QGLShaderProgram* program = GLFunctions::getShader( "colormapbar" );
    program->bind();
    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    program->setUniformValue( "u_width", (float)width );
    program->setUniformValue( "u_height", (float)height );
    program->setUniformValue( "u_scaleX", 1.0f );
    program->setUniformValue( "u_scaleY", 1.0f );

    // Draw cube geometry using indices from VBO 0
    //glDrawElements( GL_QUADS, 4, GL_UNSIGNED_SHORT, 0 );
    glDrawArrays( GL_QUADS, 0, 4 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );

    program = GLFunctions::getShader( "colormapscale" );
    program->bind();
    // Offset for position
    intptr_t offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (const void *) offset );

    program->setUniformValue( "u_width", (float)width );
    program->setUniformValue( "u_height", (float)height );
    program->setUniformValue( "u_scaleX", 1.0f );
    program->setUniformValue( "u_scaleY", 1.0f );

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );

    glDrawArrays( GL_QUADS, 0, 24 ); // third argument is count verts in buffer, not count quads

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    for ( unsigned int i = 0; i < m_labels.size(); ++i )
    {
        QString label = QString::number( m_labels[i].z(), 'f', 2 );
        float xOffset = ( ( label.size() - 3 )  * ( ( (float)m_textSize / 4 ) / (float)width ) ) * (float)width +
                        ( 0.5f  * ( ( (float)m_textSize / 4 ) / (float)width ) ) * (float)width;
        float yOffset = m_dy +  ( (float)m_textSize / 2 ) / (float)height * (float)height ;


        GLFunctions::renderText( label, m_labels[i].x() - xOffset, m_labels[i].y() - yOffset, m_textSize, width, height, m_textColor, renderMode );
    }

}

void ColormapRenderer::setX( float x )
{
    m_x = x;
    initGeometry();
}
void ColormapRenderer::setY( float y )
{
    m_y = y;
    initGeometry();
}
void ColormapRenderer::setDX( float dx )
{
    m_dx = dx;
    initGeometry();
}
void ColormapRenderer::setDY( float dy )
{
    m_dy = dy;
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

void ColormapRenderer::setTextSize( int size )
{
    m_textSize = size;
}

void ColormapRenderer::setTextColor( QColor color )
{
    m_textColor = color;
}
