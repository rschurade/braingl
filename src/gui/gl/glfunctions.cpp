/*
 * glfunctions.cpp
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */
#include "GL/glew.h"

#include "glfunctions.h"
#include "colormapfunctions.h"
#include "../../data/enums.h"
#include "../../data/models.h"

#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"


#include "shaperenderer.h"
#include "../text/textrenderer.h"

#include <QAbstractItemModel>
#include <QHash>
#include <QFile>
#include <QThread>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <locale.h>

#define NUM_TEXTURES 5

int GLFunctions::idealThreadCount = QThread::idealThreadCount();

TextRenderer* GLFunctions::m_textRenderer = new TextRenderer();
ShapeRenderer* GLFunctions::m_shapeRenderer = new ShapeRenderer();

bool GLFunctions::shadersLoaded = false;
unsigned int GLFunctions::pickIndex = 4;
QHash<QString,float> GLFunctions::sliceAlpha;

QHash< QString, QGLShaderProgram* > GLFunctions::m_shaders;
QHash< QString, QString > GLFunctions::m_shaderIncludes;
QHash< QString, QString > GLFunctions::m_shaderSources;
QVector< QString > GLFunctions::m_shaderNames;


int GLFunctions::getPickIndex()
{
    return GLFunctions::pickIndex++;
}

void GLFunctions::setupTextures( QString target )
{
    QAbstractItemModel* model = Models::d();
    QList< int > tl = getTextureIndexes( target );
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
            setTexInterpolation( tl.at( texIndex ) );
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
            setTexInterpolation( tl.at( texIndex ) );
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
            setTexInterpolation( tl.at( texIndex ) );
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
            setTexInterpolation( tl.at( texIndex ) );
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
            setTexInterpolation( tl.at( texIndex ) );
            break;
        }
        default:
            break;
    }
}

void GLFunctions::setTexInterpolation( int row )
{
    QAbstractItemModel* model = Models::d();
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
    updateColormapShader();
    for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
    {
        GLFunctions::m_shaders[ GLFunctions::m_shaderNames[ i ] ] = initShader( GLFunctions::m_shaderNames[ i ] );
    }
}

void GLFunctions::loadShaders()
{
    if ( !GLFunctions::shadersLoaded )
    {
        GLFunctions::m_shaderIncludes[ "textures_vs" ] = copyShaderToString( "textures", QString("vs") );
        GLFunctions::m_shaderIncludes[ "textures_fs" ] = copyShaderToString( "textures", QString("fs") );
        GLFunctions::m_shaderIncludes[ "lighting_vs" ] = copyShaderToString( "lighting", QString("vs") );
        GLFunctions::m_shaderIncludes[ "lighting_fs" ] = copyShaderToString( "lighting", QString("fs") );
        GLFunctions::m_shaderIncludes[ "uniforms_vs" ] = copyShaderToString( "uniforms", QString("vs") );
        GLFunctions::m_shaderIncludes[ "uniforms_fs" ] = copyShaderToString( "uniforms", QString("fs") );
        GLFunctions::m_shaderIncludes[ "peel_vs" ] = copyShaderToString( "peel", QString("vs") );
        GLFunctions::m_shaderIncludes[ "peel_fs" ] = copyShaderToString( "peel", QString("fs") );

        updateColormapShader();

        GLFunctions::m_shaderNames.push_back( "slice" );
        GLFunctions::m_shaderNames.push_back( "colormapbar" );
        GLFunctions::m_shaderNames.push_back( "fiber" );
        GLFunctions::m_shaderNames.push_back( "tube" );
        GLFunctions::m_shaderNames.push_back( "mesh" );
        GLFunctions::m_shaderNames.push_back( "colormapscale" );
        GLFunctions::m_shaderNames.push_back( "qball" );
        GLFunctions::m_shaderNames.push_back( "crosshair" );
        GLFunctions::m_shaderNames.push_back( "superquadric" );
        GLFunctions::m_shaderNames.push_back( "tensorev" );
        GLFunctions::m_shaderNames.push_back( "ev" );
        GLFunctions::m_shaderNames.push_back( "shape" );
        GLFunctions::m_shaderNames.push_back( "text" );
        GLFunctions::m_shaderNames.push_back( "merge" );
        GLFunctions::m_shaderNames.push_back( "points" );
        GLFunctions::m_shaderNames.push_back( "vectors" );
        GLFunctions::m_shaderNames.push_back( "pies" );

        for ( int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
        {
            GLFunctions::m_shaderSources[ GLFunctions::m_shaderNames[ i ] + "_vs" ] = copyShaderToString( GLFunctions::m_shaderNames[ i ], QString("vs") );
            GLFunctions::m_shaderSources[ GLFunctions::m_shaderNames[ i ] + "_fs" ] = copyShaderToString( GLFunctions::m_shaderNames[ i ], QString("fs") );
            GLFunctions::m_shaders[ GLFunctions::m_shaderNames[ i ] ] = initShader( GLFunctions::m_shaderNames[ i ] );
        }

        GLFunctions::shadersLoaded = true;
    }
}

void GLFunctions::updateColormapShader()
{
    int numColormaps = ColormapFunctions::size();
    QString code( "" );
    code += "vec4 colormap( float value, int cmap, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax, float alpha, vec4 fragColor ) \n";
    code += "{ \n";
    code += "    vec3 color = vec3(0.0); \n";
    code += "    if ( value < lowerThreshold ) \n";
    code += "    { \n";
    code += "        return fragColor; \n";
    code += "    } \n";
    code += "    if ( value > upperThreshold ) \n";
    code += "    { \n";
    code += "        return fragColor; \n";
    code += "    } \n";
    code += "    value = ( value - selectedMin ) / ( selectedMax - selectedMin ); \n";
    for ( int i = 0; i < numColormaps; ++i )
    {
        code += "    if ( cmap == " + QString::number( i ) + " ) \n";
        code += "    { \n";
        code += ColormapFunctions::get( i ).getCode();
        code += "    } \n";
    }
    code += "    return vec4( mix( fragColor.rgb, color, alpha ), 1.0 ); \n";
    code += "} \n";

    GLFunctions::m_shaderIncludes[ "colormap_fs" ] = code;
}

QString GLFunctions::copyShaderToString( QString name, QString ext )
{
    QFile file( ":/shaders/" + name + "." + ext );
    file.open( QIODevice::ReadOnly );
    QTextStream in( &file );
    QString code( "" );
    while ( !in.atEnd() )
    {
        code += in.readLine();
        code += "\n";
    }

    return code;
}

QGLShaderProgram* GLFunctions::initShader( QString name )
{
    QGLShaderProgram* program = new QGLShaderProgram;

    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    QString code = GLFunctions::m_shaderSources[ name + "_vs" ];
    QHashIterator<QString, QString> i( GLFunctions::m_shaderIncludes );
    while (i.hasNext() )
    {
        i.next();
        code = code.replace( QString( "#include " + i.key() ), QString( i.value() ) );
    }

    // Compiling vertex shader
    if ( !program->addShaderFromSourceCode( QGLShader::Vertex, code ) )
    {
        qCritical() << "Error while compiling vertex shader: " << name << "!";
        //exit( false );
    }

    code = GLFunctions::m_shaderSources[ name + "_fs" ];
    QHashIterator<QString, QString> j( GLFunctions::m_shaderIncludes );
    while (j.hasNext() )
    {
        j.next();
        code = code.replace( QString( "#include " + j.key() ), QString( j.value() ) );
    }

    // Compiling fragment shader
    if ( !program->addShaderFromSourceCode( QGLShader::Fragment, code ) )
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

void GLFunctions::setShaderVarsSlice( QGLShaderProgram* program, QString target )
{
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

    setTextureUniforms( program, target );
}
void GLFunctions::setTextureUniforms( QGLShaderProgram* program, QString target )
{
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

    QList< int > tl = getTextureIndexes( target );
    QModelIndex index;
    int texIndex = 4;
    float texMin = 0;
    float texMax = 1;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = ds->properties( target );
            texMin = props->get( Fn::Property::MIN ).toFloat();
            texMax = props->get( Fn::Property::MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold4", ( props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold4", ( props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin4", ( props->get( Fn::Property::SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax4", ( props->get( Fn::Property::SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_colormap4", props->get( Fn::Property::COLORMAP ).toInt() );
            program->setUniformValue( "u_alpha4", props->get( Fn::Property::ALPHA ).toFloat() );
            program->setUniformValue( "u_texActive4", true );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = ds->properties( target );
            texMin = props->get( Fn::Property::MIN ).toFloat();
            texMax = props->get( Fn::Property::MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold3", ( props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold3", ( props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin3", ( props->get( Fn::Property::SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax3", ( props->get( Fn::Property::SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_colormap3", props->get( Fn::Property::COLORMAP ).toInt() );
            program->setUniformValue( "u_alpha3", props->get( Fn::Property::ALPHA ).toFloat() );
            program->setUniformValue( "u_texActive3", true );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = ds->properties( target );
            texMin = props->get( Fn::Property::MIN ).toFloat();
            texMax = props->get( Fn::Property::MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold2", ( props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold2", ( props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin2", ( props->get( Fn::Property::SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax2", ( props->get( Fn::Property::SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_colormap2", props->get( Fn::Property::COLORMAP ).toInt() );
            program->setUniformValue( "u_alpha2", props->get( Fn::Property::ALPHA ).toFloat() );
            program->setUniformValue( "u_texActive2", true );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = ds->properties( target );
            texMin = props->get( Fn::Property::MIN ).toFloat();
            texMax = props->get( Fn::Property::MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold1", ( props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold1", ( props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin1", ( props->get( Fn::Property::SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax1", ( props->get( Fn::Property::SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_colormap1", props->get( Fn::Property::COLORMAP ).toInt() );
            program->setUniformValue( "u_alpha1", props->get( Fn::Property::ALPHA ).toFloat() );
            program->setUniformValue( "u_texActive1", true );

        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = ds->properties( target );
            texMin = props->get( Fn::Property::MIN ).toFloat();
            texMax = props->get( Fn::Property::MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold0", ( props->get( Fn::Property::LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold0", ( props->get( Fn::Property::UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin0", ( props->get( Fn::Property::SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax0", ( props->get( Fn::Property::SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_colormap0", props->get( Fn::Property::COLORMAP ).toInt() );
            program->setUniformValue( "u_alpha0", props->get( Fn::Property::ALPHA ).toFloat() );
            GLFunctions::sliceAlpha[target] = props->get( Fn::Property::ALPHA ).toFloat();
            program->setUniformValue( "u_texActive0", true );
        }
            break;
        default:
            break;
    }
}

QList< int > GLFunctions::getTextureIndexes( QString target )
{
    QList< int > tl;
    QAbstractItemModel* model = Models::d();
    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
        PropertyGroup* props = ds->properties( target );
        bool active = props->get( Fn::Property::ACTIVE ).toBool();
        bool isTex = props->get( Fn::Property::HAS_TEXTURE ).toBool();

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

void GLFunctions::initTextRenderer()
{
    GLFunctions::m_textRenderer->init();
}

void GLFunctions::renderText( QString text, int x, int y, int size, int width, int height, QColor color, int renderMode )
{
    GLFunctions::m_textRenderer->setSize( size );
    GLFunctions::m_textRenderer->setColor( color );
    GLFunctions::m_textRenderer->renderText( text, x, y, width, height, renderMode );
}

void GLFunctions::initShapeRenderer()
{
    GLFunctions::m_shapeRenderer->init();
}

void GLFunctions::drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                              float x, float y, float z, float dx, float dy, float dz,
                              QColor color, int pickID, int width, int height, int renderMode )
{
    GLFunctions::m_shapeRenderer->drawBox( p_matrix, mv_matrix, x, y, z, dx, dy, dz, color, pickID, width, height, renderMode );
}

void GLFunctions::drawSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                                 float x, float y, float z, float dx, float dy, float dz,
                                 QColor color, int pickID, int width, int height, int renderMode )
{
    GLFunctions::m_shapeRenderer->drawSphere( p_matrix, mv_matrix, x, y, z, dx, dy, dz, color, pickID, width, height, renderMode );
}

void GLFunctions::getAndPrintGLError( QString prefix )
{
    GLenum errCode;
    const GLubyte *errString;

    while ( ( errCode = glGetError() ) != GL_NO_ERROR )
    {
        errString = gluErrorString( errCode );
        qDebug() << "OpenGL Error:" << prefix << QString( (char*) errString );
    }
}
