/*
 * textrenderer.cpp
 *
 * Created on: Feb 20, 2013
 * @author schurade
 */

#include "textrenderer.h"

#include "../gl/glfunctions.h"

#include <QDebug>
#include <QImage>
#include <QtOpenGL/QGLShaderProgram>

TextRenderer::TextRenderer() :
    vboIds( new GLuint[ 1 ] ),
    m_fontTextureGLuint( new GLuint[ 1 ] ),
    m_textSizeX( 0.1 ),
    m_textSizeY( 0.1 )
{
    int pos = 0;
    for ( int i = 33; i < 127; ++i )
    {
        m_characterPositions[ QChar( i ) ] = pos++;
    }
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::init()
{
    initializeOpenGLFunctions();
    createFontTexture();

    glGenBuffers( 1, vboIds );
    initGeometry();
}

void TextRenderer::createFontTexture()
{
    QImage texImage( ":/icons/droidsansmono256.png", "PNG" );
    QImage GL_formatted_image = QGLWidget::convertToGLFormat(texImage);

    glGenTextures( 1, &m_fontTextureGLuint[0] );
    glBindTexture( GL_TEXTURE_2D, m_fontTextureGLuint[0] );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(), texImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void TextRenderer::renderText( QString text, int x, int y, int width, int height, int renderMode )
{
    //qDebug() << text << x << y;
    glActiveTexture( GL_TEXTURE13 );
    glBindTexture( GL_TEXTURE_2D, m_fontTextureGLuint[0] );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );

    QGLShaderProgram* program = GLFunctions::getShader( "text" );
    program->bind();

    // Offset for position
    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (const void *) offset );

    program->setUniformValue( "u_x", (float)x );
    program->setUniformValue( "u_y", (float)y );
    program->setUniformValue( "u_width", (float)width );
    program->setUniformValue( "u_height", (float)height );
    program->setUniformValue( "u_scaleX", 1.0f );
    program->setUniformValue( "u_scaleY", 1.0f );
    program->setUniformValue( "u_sizeX", m_textSizeX  / (float)width );
    program->setUniformValue( "u_sizeY", m_textSizeY  / (float)height );
    program->setUniformValue( "u_textColor", m_textColor.redF(), m_textColor.greenF(), m_textColor.blueF() );

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "fontTex", 13 );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    for ( int i = 0; i < text.size(); ++i )
    {
        QChar c = text.at( i );
        program->setUniformValue( "u_char", (float)m_characterPositions[ c ] );
        program->setUniformValue( "u_pos", (float)i );

        // XXX not in Core/deprecated //glDrawArrays( GL_QUADS, 0, 4 ); // third argument is count verts in buffer, not count quads
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    // XXX
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void TextRenderer::initGeometry()
{
    float z = -0.3;
    float vertices[] =
    {
        // XXX rearrange quad vertices (1,2,3,4) to triangle strip (1,2,4,3)
        0.0f, 0.0f, z,
        1.0f, 0.0f, z,
        0.0f, 1.0f, z,
        1.0f, 1.0f, z
    };
    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW );
}

void TextRenderer::setSize( float size )
{
    m_textSizeX = size;
    m_textSizeY = size;
}

void TextRenderer::setColor( QColor color )
{
    m_textColor = color;
}
