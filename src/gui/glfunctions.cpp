/*
 * glfunctions.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */
#include "../glew/include/glew.h"

#include <QtCore/QAbstractItemModel>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "glfunctions.h"

GLFunctions::GLFunctions()
{
}

GLFunctions::~GLFunctions()
{
}

void GLFunctions::setupTextures( QAbstractItemModel* model )
{
    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;

    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model->index( i, 1 );
        if ( model->data( index, Qt::DisplayRole ).toInt() == 1 || model->data( index, Qt::DisplayRole ).toInt() == 3 )
        {
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::UserRole ).toInt() );

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

QGLShaderProgram* GLFunctions::initSliceShader()
{
    QGLShaderProgram* program = new QGLShaderProgram;

    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !program->addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/vshader.glsl" ) )
    {
        exit( false );
    }

    // Compiling fragment shader
    if ( !program->addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/fshader.glsl" ) )
    {
        exit( false );
    }

    // Linking shader pipeline
    if ( !program->link() )
    {
        exit( false );
    }

    // Binding shader pipeline for use
    if ( !program->bind() )
    {
        exit( false );
    }

    // Restore system locale
    setlocale( LC_ALL, "" );

    return program;
}

void GLFunctions::setSliceShaderVars( QGLShaderProgram* program )
{
    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    program->setUniformValue( "texture0", 0 );
    program->setUniformValue( "texture1", 1 );
    program->setUniformValue( "texture2", 2 );
}
