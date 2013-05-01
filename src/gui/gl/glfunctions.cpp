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
float GLFunctions::sliceAlpha = 1.0;

QHash< QString, QGLShaderProgram* > GLFunctions::m_shaders;
QHash< QString, QString > GLFunctions::m_shaderIncludes;
QHash< QString, QString > GLFunctions::m_shaderSources;
QVector< QString > GLFunctions::m_shaderNames;


int GLFunctions::getPickIndex()
{
    return GLFunctions::pickIndex++;
}

void GLFunctions::setupTextures()
{
    QAbstractItemModel* model = Models::d();
    QList< int > tl = getTextureIndexes();
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

void GLFunctions::setShaderVarsSlice( QGLShaderProgram* program )
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

    setTextureUniforms( program );
}
void GLFunctions::setTextureUniforms( QGLShaderProgram* program )
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

    QList< int > tl = getTextureIndexes();
    QAbstractItemModel* model = Models::d();
    QModelIndex index;
    int texIndex = 4;
    float texMin = 0;
    float texMax = 1;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MIN );
            texMin = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold4", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold4", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin4", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax4", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
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
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MIN );
            texMin = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold3", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold3", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin3", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax3", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
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
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MIN );
            texMin = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold2", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold2", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin2", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax2", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
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
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MIN );
            texMin = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold1", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold1", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin1", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax1", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
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
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MIN );
            texMin = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::MAX );
            texMax = model->data( index, Qt::DisplayRole ).toFloat();
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::LOWER_THRESHOLD );
            program->setUniformValue( "u_lowerThreshold0", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::UPPER_THRESHOLD );
            program->setUniformValue( "u_upperThreshold0", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MIN );
            program->setUniformValue( "u_selectedMin0", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::SELECTED_MAX );
            program->setUniformValue( "u_selectedMax0", ( model->data( index, Qt::DisplayRole ).toFloat() - texMin ) / ( texMax - texMin ) );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::COLORMAP );
            program->setUniformValue( "u_colormap0", model->data( index, Qt::DisplayRole ).toInt() );
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::ALPHA );
            GLFunctions::sliceAlpha = model->data( index, Qt::DisplayRole ).toFloat();
            program->setUniformValue( "u_alpha0", GLFunctions::sliceAlpha );
            program->setUniformValue( "u_texActive0", true );

        }
            break;
        default:
            break;
    }
}

QList< int > GLFunctions::getTextureIndexes()
{
    QList< int > tl;
    QAbstractItemModel* model = Models::d();
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

GLuint GLFunctions::pbo_a = 0;
GLuint GLFunctions::pbo_b = 0;

void GLFunctions::generate_pixel_buffer_objects( int width, int height )
{
    /* generate the first pixel buffer objects */
    glGenBuffers( 1, &GLFunctions::pbo_a );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, GLFunctions::pbo_a );
    glBufferData( GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_STREAM_READ );
    /* to avoid weird behaviour the first frame the data is loaded */
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* generate the second pixel buffer objects */
    glGenBuffers( 1, &GLFunctions::pbo_b );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, GLFunctions::pbo_b );
    glBufferData( GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_STREAM_READ );
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* unbind */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
}

uint GLFunctions::get_object_id( int x, int y, int width, int height )
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::FBO );
    glReadBuffer( GL_COLOR_ATTACHMENT2 );

    static int frame_event = 0;
    int read_pbo, map_pbo;
    uint object_id;

    uint red, green, blue, pixel_index;
    GLubyte* ptr;

    GLFunctions::generate_pixel_buffer_objects( width, height );
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = GLFunctions::pbo_a;
        map_pbo = GLFunctions::pbo_b;
    }
    else {
        frame_event = 0;
        read_pbo = GLFunctions::pbo_b;
        map_pbo = GLFunctions::pbo_a;

    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */
    y = height - y;
    object_id = -1;

    if ( x >= 0 && x < width && y >= 0 && y < height )
    {
        pixel_index = ( x + y * width ) * 4;
        blue = ptr[pixel_index];
        green = ptr[pixel_index + 1];
        red = ptr[pixel_index + 2];
        //alpha = ptr[pixel_index + 3];
        //object_id = alpha + ( red << 24 ) + ( green << 16 ) + ( blue << 8 );
        object_id = ( red << 16 ) + ( green << 8 ) + ( blue );
        //qDebug() << "output" << red << green << blue << alpha;
    }
    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

    glDeleteBuffers( 1, &GLFunctions::pbo_a );
    glDeleteBuffers( 1, &GLFunctions::pbo_b );

    return object_id;
}

QImage* GLFunctions::getOffscreenTexture( int width, int height )
{
    static int frame_event = 0;
    int read_pbo, map_pbo;

    uint red, green, blue, alpha, pixel_index;
    GLubyte* ptr;

    GLFunctions::generate_pixel_buffer_objects( width, height );
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = GLFunctions::pbo_a;
        map_pbo = GLFunctions::pbo_a;
    }
    else {
        frame_event = 0;
        map_pbo = GLFunctions::pbo_a;
        read_pbo = GLFunctions::pbo_b;
    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */

    QImage* image = new QImage( width, height, QImage::Format_RGB32 );
    QColor c;
    for ( int x = 0; x < width; ++x )
    {
        for ( int y = 0; y < height; ++y )
        {
            pixel_index = ( x + y * width ) * 4;
            blue = ptr[pixel_index];
            green = ptr[pixel_index + 1];
            red = ptr[pixel_index + 2];
            alpha = ptr[pixel_index + 3];

            c = QColor( red, green, blue, alpha );
            image->setPixel( x, ( height - y )-1, c.rgba() );
        }
    }

    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

    glDeleteBuffers( 1, &GLFunctions::pbo_a );
    glDeleteBuffers( 1, &GLFunctions::pbo_b );

    return image;
}


QHash< QString, GLuint > GLFunctions::textures;
GLuint GLFunctions::RBO = 0;
GLuint GLFunctions::FBO = 0;


void GLFunctions::initFBO( int width, int height )
{
    if ( GLFunctions::textures.size() > 0 )
    {
        foreach ( GLuint tex, GLFunctions::textures )
        {
            glDeleteTextures( 1, &tex );
        }
        glDeleteFramebuffers( 1, &GLFunctions::FBO );
        glDeleteRenderbuffers( 1, &GLFunctions::RBO );

    }

    GLFunctions::textures[ "C0" ] = createTexture( width, height );
    GLFunctions::textures[ "C1" ] = createTexture( width, height );
    GLFunctions::textures[ "C2" ] = createTexture( width, height );
    GLFunctions::textures[ "C3" ] = createTexture( width, height );
    GLFunctions::textures[ "D0" ] = createTexture( width, height );
    GLFunctions::textures[ "D1" ] = createTexture( width, height );
    GLFunctions::textures[ "D2" ] = createTexture( width, height );
    GLFunctions::textures[ "PICK" ] = createTexture( width, height );
    GLFunctions::textures[ "SCREENSHOT" ] = createTexture( width, height );

    /* create a framebuffer object */
    glGenFramebuffers( 1, &GLFunctions::FBO );
    /* attach the texture and the render buffer to the frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::FBO );

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3,  attachments );

    /* create a renderbuffer object for the depth buffer */
    glGenRenderbuffers( 1, &GLFunctions::RBO );
    /* bind the texture */
    glBindRenderbuffer( GL_RENDERBUFFER, GLFunctions::RBO );
    /* create the render buffer in the GPU */
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLFunctions::textures["C0"], 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GLFunctions::RBO );
    /* check the frame buffer */
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        /* handle an error : frame buffer incomplete */
        qDebug() << "frame buffer incomplete";
    }

    /* unbind the render buffer */
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    /* return to the default frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

GLuint GLFunctions::createTexture( int width, int height )
{
    /* generate a texture id */
    GLuint ptex;
    glGenTextures( 1, &ptex );
    /* bind the texture */
    glBindTexture( GL_TEXTURE_2D, ptex );
    /* set texture parameters */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    /* create the texture in the GPU */
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    /* unbind the texture */
    glBindTexture( GL_TEXTURE_2D, 0 );

    return ptex;
}

void GLFunctions::setRenderTarget( QString target )
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLFunctions::textures[target], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GLFunctions::textures["PICK"], 0 );
}

void GLFunctions::setRenderTargets( QString target0, QString target1 )
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLFunctions::textures[target0], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GLFunctions::textures[target1], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GLFunctions::textures["PICK"], 0 );
}

void GLFunctions::clearTexture( QString target, float r, float g, float b, float a )
{
    glBindFramebuffer( GL_FRAMEBUFFER, GLFunctions::FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLFunctions::textures[target], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
    glClearColor( r, g, b, a );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

GLuint GLFunctions::getTexture( QString name )
{
    return GLFunctions::textures[name];
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
