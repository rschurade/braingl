/*
 * glfunctions.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */
#include "../../glew/include/glew.h"

#include <QtCore/QAbstractItemModel>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "glfunctions.h"

#define NUM_TEXTURES 5

GLFunctions::GLFunctions()
{
}

GLFunctions::~GLFunctions()
{
}

void GLFunctions::setupTextures( QAbstractItemModel* model )
{
    QList<int> tl = getTextureIndexes( model );
    QModelIndex index;
    int texIndex = 4;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ), 1 );
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::EditRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE4 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            index = model->index( tl.at( texIndex ), 1 );
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::EditRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE3 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            index = model->index( tl.at( texIndex ), 1 );
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::EditRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE2 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            index = model->index( tl.at( texIndex ), 1 );
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::EditRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            index = model->index( tl.at( texIndex ), 1 );
            GLuint tex = static_cast<GLuint>( model->data( index, Qt::EditRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
            break;
        }
        default:
            break;
    }
}

void GLFunctions::setTexInterpolation( QAbstractItemModel* model, int row )
{
    QModelIndex index = model->index( row, 53 );
    bool interpolation = model->data( index, Qt::EditRole ).toBool();
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
}

QGLShaderProgram* GLFunctions::initShader( QString name )
{
    QGLShaderProgram* program = new QGLShaderProgram;

    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !program->addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/" + name + ".vs" ) )
    {
        exit( false );
    }

    // Compiling fragment shader
    if ( !program->addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/" + name + ".fs" ) )
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

void GLFunctions::setSliceShaderVars( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();
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
    program->setUniformValue( "texture3", 3 );
    program->setUniformValue( "texture4", 4 );

    program->setUniformValue( "u_texActive0", false );
    program->setUniformValue( "u_texActive1", false );
    program->setUniformValue( "u_texActive2", false );
    program->setUniformValue( "u_texActive3", false );
    program->setUniformValue( "u_texActive4", false );


    QList<int> tl = getTextureIndexes( model );
    QModelIndex index;
    int texIndex = 4;
    float texMax = 1;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ), 11 );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ), 50 );
            program->setUniformValue( "u_lowerThreshold4", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 51 );
            program->setUniformValue( "u_upperThreshold4", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 52 );
            program->setUniformValue( "u_colormap4", model->data( index, Qt::EditRole ).toInt() );
            index = model->index( tl.at( texIndex ), 54 );
            program->setUniformValue( "u_alpha4", model->data( index, Qt::EditRole ).toFloat() );
            program->setUniformValue( "u_texActive4", true );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            index = model->index( tl.at( texIndex ), 11 );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ), 50 );
            program->setUniformValue( "u_lowerThreshold3", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 51 );
            program->setUniformValue( "u_upperThreshold3", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 52 );
            program->setUniformValue( "u_colormap3", model->data( index, Qt::EditRole ).toInt() );
            index = model->index( tl.at( texIndex ), 54 );
            program->setUniformValue( "u_alpha3", model->data( index, Qt::EditRole ).toFloat() );
            program->setUniformValue( "u_texActive3", true );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            index = model->index( tl.at( texIndex ), 11 );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ), 50 );
            program->setUniformValue( "u_lowerThreshold2", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 51 );
            program->setUniformValue( "u_upperThreshold2", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 52 );
            program->setUniformValue( "u_colormap2", model->data( index, Qt::EditRole ).toInt() );
            index = model->index( tl.at( texIndex ), 54 );
            program->setUniformValue( "u_alpha2", model->data( index, Qt::EditRole ).toFloat() );
            program->setUniformValue( "u_texActive2", true );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            index = model->index( tl.at( texIndex ), 11 );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ), 50 );
            program->setUniformValue( "u_lowerThreshold1", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 51 );
            program->setUniformValue( "u_upperThreshold1", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 52 );
            program->setUniformValue( "u_colormap1", model->data( index, Qt::EditRole ).toInt() );
            index = model->index( tl.at( texIndex ), 54 );
            program->setUniformValue( "u_alpha1", model->data( index, Qt::EditRole ).toFloat() );
            program->setUniformValue( "u_texActive1", true );
        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            index = model->index( tl.at( texIndex ), 11 );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ), 50 );
            program->setUniformValue( "u_lowerThreshold0", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 51 );
            program->setUniformValue( "u_upperThreshold0", model->data( index, Qt::EditRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ), 52 );
            program->setUniformValue( "u_colormap0", model->data( index, Qt::EditRole ).toInt() );
            index = model->index( tl.at( texIndex ), 54 );
            program->setUniformValue( "u_alpha0", model->data( index, Qt::EditRole ).toFloat() );
            program->setUniformValue( "u_texActive0", true );

        }
            break;
        default:
            break;
    }
}

QList<int> GLFunctions::getTextureIndexes( QAbstractItemModel* model )
{
    QList<int>tl;

    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model->index( i, 1 );
        if ( model->data( index, Qt::DisplayRole ).toInt() == 1 || model->data( index, Qt::DisplayRole ).toInt() == 3 )
        {
            index = model->index( i, 55 );
            if ( model->data( index, Qt::EditRole ).toBool() )
            {
                tl.push_back( i );
                ++allocatedTextureCount;
                if ( allocatedTextureCount >= NUM_TEXTURES )
                {
                    break;
                }
            }
        }
    }

    return tl;
}
