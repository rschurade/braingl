/*
 * slicerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../glew/include/glew.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "slicerenderer.h"

SliceRenderer::SliceRenderer() :
    ObjectRenderer(),
    m_program( new QGLShaderProgram ),
    vboIds( new GLuint[ 4 ] ),
    m_x( 0. ),
    m_y( 0.),
    m_z( 0.),
    m_xb( 0 ),
    m_yb( 0 ),
    m_zb( 0 )
{
}

SliceRenderer::~SliceRenderer()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void SliceRenderer::init()
{
    initShader();
    initGeometry();
}

void SliceRenderer::initShader()
{
    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/vshader.glsl" ) )
    {
        exit( false );
    }

    // Compiling fragment shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/fshader.glsl" ) )
    {
        exit( false );
    }

    // Linking shader pipeline
    if ( !m_program->link() )
    {
        exit( false );
    }

    // Binding shader pipeline for use
    if ( !m_program->bind() )
    {
        exit( false );
    }

    // Restore system locale
    setlocale( LC_ALL, "" );
}

void SliceRenderer::initGeometry()
{
    glGenBuffers( 4, vboIds );

    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    m_xb = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toFloat();

    float dx = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

    m_x *= dx;
    m_y *= dy;
    m_z *= dz;
    m_xb *= dx;
    m_yb *= dy;
    m_zb *= dz;


    VertexData verticesAxial[] =
    {
            { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) },
            { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) }
    };

    VertexData verticesCoronal[] =
    {
            { QVector3D( 0.0,  m_y, 0.0  ), QVector3D( 0.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_y, 0.0  ), QVector3D( 1.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_y, m_zb ), QVector3D( 1.0, m_y/m_yb, 1.0 ) },
            { QVector3D( 0.0,  m_y, m_zb ), QVector3D( 0.0, m_y/m_yb, 1.0 ) }
    };

    VertexData verticesSagittal[] =
    {
            { QVector3D( m_x, 0.0,  0.0  ), QVector3D( m_x/m_xb, 0.0, 0.0 ) },
            { QVector3D( m_x, m_yb, 0.0  ), QVector3D( m_x/m_xb, 1.0, 0.0 ) },
            { QVector3D( m_x, m_yb, m_zb ), QVector3D( m_x/m_xb, 1.0, 1.0 ) },
            { QVector3D( m_x, 0.0,  m_zb ), QVector3D( m_x/m_xb, 0.0, 1.0 ) }
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 2
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );

}

void SliceRenderer::setupTextures()
{
    int countDatasets = model()->rowCount();
    int allocatedTextureCount = 0;

    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model()->index( i, 1 );
        if ( model()->data( index, Qt::DisplayRole ).toInt() == 1 || model()->data( index, Qt::DisplayRole ).toInt() == 3 )
        {
            GLuint tex = static_cast<GLuint>( model()->data( index, Qt::UserRole ).toInt() );

            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

            glActiveTexture( GL_TEXTURE0 + allocatedTextureCount );
            glBindTexture( GL_TEXTURE_3D, tex );

            bool interpolation = false;
            if ( interpolation )
            {
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            }
            else
            {
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            }
            ++allocatedTextureCount;
        }
    }
}

void SliceRenderer::setShaderVars()
{
    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation( "a_position" );
    m_program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    int samplerUniform0 = m_program->attributeLocation( "texture0" );
    int samplerUniform1 = m_program->attributeLocation( "texture1" );
    int samplerUniform2 = m_program->attributeLocation( "texture2" );

    m_program->setUniformValue( samplerUniform0, 0 );
    m_program->setUniformValue( samplerUniform1, 1 );
    m_program->setUniformValue( samplerUniform2, 2 );
}

void SliceRenderer::draw( QMatrix4x4 mvp_matrix )
{
    setupTextures();

    glColor4f( 0.0, 0.0, 0.0, 1.0 );


    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", mvp_matrix );

    initGeometry();

    drawAxial();
    drawCoronal();
    drawSagittal();

    glDeleteBuffers( 1, &vboIds[0] );
    glDeleteBuffers( 1, &vboIds[1] );
    glDeleteBuffers( 1, &vboIds[2] );
    glDeleteBuffers( 1, &vboIds[3] );
}

void SliceRenderer::drawAxial()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void SliceRenderer::drawCoronal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void SliceRenderer::drawSagittal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}
