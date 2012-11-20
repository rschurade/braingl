/*
 * glfunctions.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "glfunctions.h"

#define NUM_TEXTURES 5

bool GLFunctions::shadersLoaded = false;
QHash<QString, QGLShaderProgram*> GLFunctions::m_shaders;
QHash<QString, QString> GLFunctions::m_shaderSources;
QVector<QString> GLFunctions::m_shaderNames;

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

QGLShaderProgram* GLFunctions::getShader( QString name )
{
    return m_shaders[name];
}

QVector<QString> GLFunctions::getShaderNames()
{
    return m_shaderNames;
}

QString GLFunctions::getShaderCode( QString name )
{
    return m_shaderSources[name];
}

void GLFunctions::setShaderCode( QString name,  QString source )
{
    GLFunctions::m_shaderSources[name] = source;
    reloadShaders();
}

void GLFunctions::reloadShaders()
{
    for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
    {
        GLFunctions::m_shaders[GLFunctions::m_shaderNames[i]] = initShader( GLFunctions::m_shaderNames[i] );
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

        for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
        {
            copyShaderToString( GLFunctions::m_shaderNames[i] );
            GLFunctions::m_shaders[GLFunctions::m_shaderNames[i]] = initShader( GLFunctions::m_shaderNames[i] );
        }

        GLFunctions::shadersLoaded = true;
    }
}

void GLFunctions::copyShaderToString( QString name )
{
    QFile fileVS( ":/shaders/" + name + ".vs" );
    fileVS.open(QIODevice::ReadOnly);
    QTextStream inVS( &fileVS );
    QString codeVS( "" );
    while ( !inVS.atEnd() )
    {
        codeVS += inVS.readLine();
        codeVS += "\n";
    }
    GLFunctions::m_shaderSources[name + "_vs"] = codeVS;
    QFile fileFS( ":/shaders/" + name + ".fs" );
    fileFS.open(QIODevice::ReadOnly);
    QTextStream inFS( &fileFS );
    QString codeFS( "" );
    while ( !inFS.atEnd() )
    {
        codeFS += inFS.readLine();
        codeFS += "\n";
    }
    GLFunctions::m_shaderSources[name + "_fs"] = codeFS;
}

QGLShaderProgram* GLFunctions::initShader( QString name )
{
    QGLShaderProgram* program = new QGLShaderProgram;

    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !program->addShaderFromSourceCode( QGLShader::Vertex, GLFunctions::m_shaderSources[name + "_vs"] ) )
    {
        qCritical() << "Error while compiling vertex shader: " << name << "!";
        //exit( false );
    }

    // Compiling fragment shader
    if ( !program->addShaderFromSourceCode( QGLShader::Fragment, GLFunctions::m_shaderSources[name + "_fs"] ) )
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
        setSliceShaderVars( m_shaders[name], model );
    }
    if ( name == "qball" )
    {
        setQBallShaderVars( m_shaders[name], model );
    }
    if ( name == "superquadric" )
    {
        setTensorShaderVars( m_shaders[name], model );
    }
    if ( name == "tensorev" )
    {
        setTensorShaderVarsEV( m_shaders[name], model );
    }
    if ( name == "ev" )
    {
        setShaderVarsEV( m_shaders[name], model );
    }
}

void GLFunctions::setTensorShaderVars( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float)*3;
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)offset );

    offset += sizeof(float)*3;
    int offsetLocation = program->attributeLocation("a_diag");
    program->enableAttributeArray(offsetLocation);
    glVertexAttribPointer(offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)offset );

    offset += sizeof(float)*3;
    int radiusLocation = program->attributeLocation("a_offdiag");
    program->enableAttributeArray(radiusLocation);
    glVertexAttribPointer(radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *)offset );
}

void GLFunctions::setTensorShaderVarsEV( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    offset += sizeof(float)*3;
    int dirLocation = program->attributeLocation("a_dir");
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer(dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );

    offset += sizeof(float)*1;
    int offsetLocation = program->attributeLocation("a_diag");
    program->enableAttributeArray(offsetLocation);
    glVertexAttribPointer(offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );

    offset += sizeof(float)*3;
    int radiusLocation = program->attributeLocation("a_offdiag");
    program->enableAttributeArray(radiusLocation);
    glVertexAttribPointer(radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );
}

void GLFunctions::setShaderVarsEV( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float)*3;
    int dirLocation = program->attributeLocation("a_dir");
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer(dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *)offset );

    offset += sizeof(float)*1;
    int offsetLocation = program->attributeLocation("a_vec");
    program->enableAttributeArray(offsetLocation);
    glVertexAttribPointer(offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *)offset );
}

void GLFunctions::setSliceShaderVars( QGLShaderProgram* program, QAbstractItemModel* model )
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

void GLFunctions::setQBallShaderVars( QGLShaderProgram* program, QAbstractItemModel* model )
{
    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(float)*3;

    // Tell OpenGL programmable pipeline how to locate vertex normal data
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );

    offset += sizeof(float)*3;
    int offsetLocation = program->attributeLocation("a_offset");
    program->enableAttributeArray(offsetLocation);
    glVertexAttribPointer(offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );

    offset += sizeof(float)*3;
    int radiusLocation = program->attributeLocation("a_radius");
    program->enableAttributeArray(radiusLocation);
    glVertexAttribPointer(radiusLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *)offset );
}

QList<int> GLFunctions::getTextureIndexes( QAbstractItemModel* model )
{
    QList<int>tl;

    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model->index( i, 1 );

        bool active = model->data( model->index( i, 55 ), Qt::EditRole ).toBool();
        int dim = model->data( model->index( i, 1 ), Qt::DisplayRole ).toInt();
        bool isTex = ( dim == 1 || dim == 3 );
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
