/*
 * glfunctions.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph Schurade
 */
#include "../../thirdparty/glew/include/glew.h"

#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#define NUM_TEXTURES 5

bool GLFunctions::shadersLoaded = false;
unsigned int GLFunctions::pickIndex = 1;
bool GLFunctions::picking = false;

QHash< QString, QGLShaderProgram* > GLFunctions::m_shaders;
QHash< QString, QString > GLFunctions::m_shaderSources;
QVector< QString > GLFunctions::m_shaderNames;

GLFunctions::GLFunctions()
{
}

GLFunctions::~GLFunctions()
{
}

int GLFunctions::getPickIndex()
{
    return GLFunctions::pickIndex++;
}

void GLFunctions::setupTextures( QAbstractItemModel* model )
{
    QList< int > tl = getTextureIndexes( model );
    QModelIndex index;
    int texIndex = 4;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE4 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE3 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE2 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( model, tl.at( texIndex ) );
        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
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
    QModelIndex index = model->index( row,  (int)Fn::Property::INTERPOLATION );
    bool interpolation = model->data( index, Qt::DisplayRole ).toBool();
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

QGLShaderProgram* GLFunctions::getShader( QString name )
{
    return m_shaders[ name ];
}

QVector< QString > GLFunctions::getShaderNames()
{
    return m_shaderNames;
}

QString GLFunctions::getShaderCode( QString name )
{
    return m_shaderSources[ name ];
}

void GLFunctions::setShaderCode( QString name, QString source )
{
    GLFunctions::m_shaderSources[ name ] = source;
    reloadShaders();
}

void GLFunctions::reloadShaders()
{
    for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
    {
        GLFunctions::m_shaders[ GLFunctions::m_shaderNames[ i ] ] = initShader( GLFunctions::m_shaderNames[ i ] );
    }
}

void GLFunctions::loadShaders()
{
    if ( !GLFunctions::shadersLoaded )
    {
        GLFunctions::m_shaderNames.push_back( "slice" );
        GLFunctions::m_shaderNames.push_back( "qball" );
        GLFunctions::m_shaderNames.push_back( "crosshair" );
        GLFunctions::m_shaderNames.push_back( "superquadric" );
        GLFunctions::m_shaderNames.push_back( "tensorev" );
        GLFunctions::m_shaderNames.push_back( "ev" );
        GLFunctions::m_shaderNames.push_back( "mesh" );
        GLFunctions::m_shaderNames.push_back( "fiber" );
        GLFunctions::m_shaderNames.push_back( "box" );

        for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
        {
            copyShaderToString( GLFunctions::m_shaderNames[ i ] );
            GLFunctions::m_shaders[ GLFunctions::m_shaderNames[ i ] ] = initShader( GLFunctions::m_shaderNames[ i ] );
        }

        GLFunctions::shadersLoaded = true;
    }
}

void GLFunctions::copyShaderToString( QString name )
{
    QFile fileVS( ":/shaders/" + name + ".vs" );
    fileVS.open( QIODevice::ReadOnly );
    QTextStream inVS( &fileVS );
    QString codeVS( "" );
    while ( !inVS.atEnd() )
    {
        codeVS += inVS.readLine();
        codeVS += "\n";
    }
    GLFunctions::m_shaderSources[ name + "_vs" ] = codeVS;
    QFile fileFS( ":/shaders/" + name + ".fs" );
    fileFS.open( QIODevice::ReadOnly );
    QTextStream inFS( &fileFS );
    QString codeFS( "" );
    while ( !inFS.atEnd() )
    {
        codeFS += inFS.readLine();
        codeFS += "\n";
    }
    GLFunctions::m_shaderSources[ name + "_fs" ] = codeFS;
}

QGLShaderProgram* GLFunctions::initShader( QString name )
{
    QGLShaderProgram* program = new QGLShaderProgram;

    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !program->addShaderFromSourceCode( QGLShader::Vertex, GLFunctions::m_shaderSources[ name + "_vs" ] ) )
    {
        qCritical() << "Error while compiling vertex shader: " << name << "!";
        //exit( false );
    }

    // Compiling fragment shader
    if ( !program->addShaderFromSourceCode( QGLShader::Fragment, GLFunctions::m_shaderSources[ name + "_fs" ] ) )
    {
        qCritical() << "Error while compiling fragment shader: " << name << "!";
        //exit( false );
    }

    // Linking shader pipeline
    if ( !program->link() )
    {
        qCritical() << "Error while linking shader: " << name << "!";
        //exit( false );
    }

    // Binding shader pipeline for use
    if ( !program->bind() )
    {
        qCritical() << "Error while binding shader: " << name << "!";
        //exit( false );
    }

    // Restore system locale
    setlocale( LC_ALL, "" );

    return program;
}

void GLFunctions::setShaderVars( QString name, QAbstractItemModel* model )
{
    if ( name == "slice" )
    {
        setShaderVarsSlice( m_shaders[ name ], model );
    }
    if ( name == "qball" )
    {
        setShaderVarsQBall( m_shaders[ name ], model );
    }
    if ( name == "superquadric" )
    {
        setShaderVarsTensor( m_shaders[ name ], model );
    }
    if ( name == "tensorev" )
    {
        setShaderVarsTensorEV( m_shaders[ name ], model );
    }
    if ( name == "ev" )
    {
        setShaderVarsEV( m_shaders[ name ], model );
    }
    if ( name == "mesh" )
    {
        setShaderVarsMesh( m_shaders[ name ], model );
    }
    if ( name == "fiber" )
    {
        setShaderVarsFiber( m_shaders[ name ], model );
    }
    if ( name == "box" )
    {
        setShaderVarsBox( m_shaders[ name ] );
    }
}

void GLFunctions::setShaderVarsMesh( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void *) offset );
}

void GLFunctions::setShaderVarsFiber( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (const void *) offset );

    offset += sizeof(float) * 3;
    int colorLocation = program->attributeLocation( "a_color" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (const void *) offset );
}

void GLFunctions::setShaderVarsTensor( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_diag" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_offdiag" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );
}

void GLFunctions::setShaderVarsTensorEV( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int dirLocation = program->attributeLocation( "a_dir" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 1;
    int offsetLocation = program->attributeLocation( "a_diag" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_offdiag" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );
}

void GLFunctions::setShaderVarsEV( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 3;
    int dirLocation = program->attributeLocation( "a_dir" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 1;
    int offsetLocation = program->attributeLocation( "a_vec" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );
}

void GLFunctions::setShaderVarsBox( QGLShaderProgram* program )
{
    program->bind();
    // Offset for position
    long int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (const void *) offset );
}

void GLFunctions::setShaderVarsSlice( QGLShaderProgram* program, QAbstractItemModel* model )
{
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

    QList< int > tl = getTextureIndexes( model );
    QModelIndex index;
    int texIndex = 4;
    float texMax = 1;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold4", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold4", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin4", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax4", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap4", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            program->setUniformValue( "u_alpha4", model->data( index, Qt::DisplayRole ).toFloat() );
            program->setUniformValue( "u_texActive4", true );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold3", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold3", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin3", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax3", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap3", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            program->setUniformValue( "u_alpha3", model->data( index, Qt::DisplayRole ).toFloat() );
            program->setUniformValue( "u_texActive3", true );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold2", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold2", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin2", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax2", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap2", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            program->setUniformValue( "u_alpha2", model->data( index, Qt::DisplayRole ).toFloat() );
            program->setUniformValue( "u_texActive2", true );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold1", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold1", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin1", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax1", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap1", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            program->setUniformValue( "u_alpha1", model->data( index, Qt::DisplayRole ).toFloat() );
            program->setUniformValue( "u_texActive1", true );
        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold0", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold0", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin0", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax0", model->data( index, Qt::DisplayRole ).toFloat() / texMax );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap0", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            program->setUniformValue( "u_alpha0", model->data( index, Qt::DisplayRole ).toFloat() );
            program->setUniformValue( "u_texActive0", true );

        }
            break;
        default:
            break;
    }
}

void GLFunctions::setShaderVarsQBall( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(float) * 3;

    // Tell OpenGL programmable pipeline how to locate vertex normal data
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_offset" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_radius" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );
}

QList< int > GLFunctions::getTextureIndexes( QAbstractItemModel* model )
{
    QList< int > tl;

    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model->index( i, 1 );

        bool active = model->data( model->index( i,  (int)Fn::Property::ACTIVE ), Qt::DisplayRole ).toBool();
        bool isTex = model->data( model->index( i,  (int)Fn::Property::HAS_TEXTURE ), Qt::DisplayRole ).toBool();
        if ( active && isTex )
        {
            tl.push_back( i );
            ++allocatedTextureCount;
            if ( allocatedTextureCount >= NUM_TEXTURES )
            {
                break;
            }
        }
    }

    return tl;
}

GLuint GLFunctions::tex = 0;
GLuint GLFunctions::rbo = 0;
GLuint GLFunctions::fbo = 0;
GLuint GLFunctions::pbo_a = 0;
GLuint GLFunctions::pbo_b = 0;
int GLFunctions::screenWidth = 0;
int GLFunctions::screenHeight = 0;

void GLFunctions::generate_frame_buffer_texture( const int screen_width, const int screen_height )
{
    GLFunctions::screenWidth = screen_width;
    GLFunctions::screenHeight = screen_height;

    /* generate a texture id */
    glGenTextures( 1, &GLFunctions::tex );
    /* bind the texture */
    glBindTexture( GL_TEXTURE_2D, GLFunctions::tex );
    /* set texture parameters */
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    /* create the texture in the GPU */
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
    /* unbind the texture */
    glBindTexture( GL_TEXTURE_2D, 0 );
    /* create a renderbuffer object for the depth buffer */
    glGenRenderbuffers( 1, &GLFunctions::rbo );
    /* bind the texture */
    glBindRenderbuffer( GL_RENDERBUFFER, GLFunctions::rbo );
    /* create the render buffer in the GPU */
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height );
    /* unbind the render buffer */
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    /* create a framebuffer object */
    glGenFramebuffers( 1, &GLFunctions::fbo );
    /* attach the texture and the render buffer to the frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::fbo );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLFunctions::tex, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GLFunctions::rbo );
    /* check the frame buffer */
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        /* handle an error : frame buffer incomplete */
    }

    /* return to the default frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GLFunctions::generate_pixel_buffer_objects()
{
    /* generate the first pixel buffer objects */
    glGenBuffers( 1, &GLFunctions::pbo_a );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, GLFunctions::pbo_a );
    glBufferData( GL_PIXEL_PACK_BUFFER, GLFunctions::screenWidth * GLFunctions::screenHeight * 4, NULL, GL_STREAM_READ );
    /* to avoid weird behaviour the first frame the data is loaded */
    glReadPixels( 0, 0, GLFunctions::screenWidth, GLFunctions::screenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* generate the second pixel buffer objects */
    glGenBuffers( 1, &GLFunctions::pbo_b );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, GLFunctions::pbo_b );
    glBufferData( GL_PIXEL_PACK_BUFFER, GLFunctions::screenWidth * GLFunctions::screenHeight * 4, NULL, GL_STREAM_READ );
    glReadPixels( 0, 0, GLFunctions::screenWidth, GLFunctions::screenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* unbind */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
}

uint GLFunctions::get_object_id( int x, int y )
{
    static int frame_event = 0;
    int read_pbo, map_pbo;
    uint object_id;

    uint red, green, blue, alpha, pixel_index;
    GLubyte* ptr;

    GLFunctions::generate_pixel_buffer_objects();
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = GLFunctions::pbo_b;
        map_pbo = GLFunctions::pbo_a;
    }
    else {
        frame_event = 0;
        map_pbo = GLFunctions::pbo_a;
        read_pbo = GLFunctions::pbo_b;
    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, GLFunctions::screenWidth, GLFunctions::screenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */
    y = GLFunctions::screenHeight - y;
    object_id = -1;

    if ( x >= 0 && x < GLFunctions::screenWidth && y >= 0 && y < GLFunctions::screenHeight )
    {
        pixel_index = ( x + y * GLFunctions::screenWidth ) * 4;
        blue = ptr[pixel_index];
        green = ptr[pixel_index + 1];
        red = ptr[pixel_index + 2];
        alpha = ptr[pixel_index + 3];
        //object_id = alpha + ( red << 24 ) + ( green << 16 ) + ( blue << 8 );
        object_id = ( red << 16 ) + ( green << 8 ) + ( blue );
        //qDebug() << "output" << red << green << blue << alpha;
    }
    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
    return object_id;
}

void GLFunctions::beginPicking()
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::fbo );
    GLFunctions::picking = true;
}

void GLFunctions::endPicking()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    GLFunctions::picking = false;
}

bool GLFunctions::isPicking()
{
    return GLFunctions::picking;
}

void GLFunctions::beginOffscreen()
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::fbo );
}

void GLFunctions::endOffscreen()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

QImage* GLFunctions::getOffscreenTexture()
{
    static int frame_event = 0;
    int read_pbo, map_pbo;

    uint red, green, blue, alpha, pixel_index;
    GLubyte* ptr;

    GLFunctions::generate_pixel_buffer_objects();
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = GLFunctions::pbo_b;
        map_pbo = GLFunctions::pbo_a;
    }
    else {
        frame_event = 0;
        map_pbo = GLFunctions::pbo_a;
        read_pbo = GLFunctions::pbo_b;
    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, GLFunctions::screenWidth, GLFunctions::screenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */

    QImage* image = new QImage( GLFunctions::screenWidth, GLFunctions::screenHeight, QImage::Format_RGB32 );
    QColor c;
    for ( int x = 0; x < GLFunctions::screenWidth; ++x )
    {
        for ( int y = 0; y < GLFunctions::screenHeight; ++y )
        {
            pixel_index = ( x + y * GLFunctions::screenWidth ) * 4;
            blue = ptr[pixel_index];
            green = ptr[pixel_index + 1];
            red = ptr[pixel_index + 2];
            alpha = ptr[pixel_index + 3];

            c = QColor( red, green, blue, alpha );
            image->setPixel( x, ( GLFunctions::screenHeight - y )-1, c.rgba() );
        }
    }

    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

    return image;
}
