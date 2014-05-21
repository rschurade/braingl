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
#include "../../data/roi.h"

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

#if defined(Q_OS_MAC) && QT_VERSION <= 0x040806 && QT_VERSION >= 0x040800    // if less or equal to 4.8.6
#include "bugfixglshaderprogram.h"
#endif

#define NUM_TEXTURES 5

int GLFunctions::idealThreadCount = qMax( 1, QThread::idealThreadCount() - 1 );
int GLFunctions::maxDim = 250;

TextRenderer* GLFunctions::m_textRenderer = new TextRenderer();
ShapeRenderer* GLFunctions::m_shapeRenderer = new ShapeRenderer();

bool GLFunctions::shadersLoaded = false;
unsigned int GLFunctions::pickIndex = 100;
QHash<QString,float> GLFunctions::sliceAlpha;

QHash< QString, QGLShaderProgram* > GLFunctions::m_shaders;
QHash< QString, QString > GLFunctions::m_shaderIncludes;
QHash< QString, QString > GLFunctions::m_shaderSources;
std::vector< QString > GLFunctions::m_shaderNames;
std::vector< GLuint > GLFunctions::m_texturesToDelete;

ROI* GLFunctions::roi = 0;

unsigned int GLFunctions::getPickIndex()
{
    return GLFunctions::pickIndex++;
}

bool GLFunctions::setupTextures( QString target )
{
    for ( unsigned int i = 0; i < GLFunctions::m_texturesToDelete.size(); ++i )
    {
        glDeleteTextures( 1, &GLFunctions::m_texturesToDelete[i] );
    }
    GLFunctions::m_texturesToDelete.clear();

    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glActiveTexture( GL_TEXTURE15 );
    glBindTexture( GL_TEXTURE_3D, 0 );

    QAbstractItemModel* model = Models::d();
    QList< int > tl = getTextureIndexes( target );
    if ( tl.empty() )
    {
        GLFunctions::sliceAlpha["maingl"] = 0.0f;
        return false;
    }

    QModelIndex index;
    int texIndex = 4;
    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::D_TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            // XXX not in Core //glf.glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE4 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( tl.at( texIndex ) );
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::D_TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            // XXX not in Core //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE3 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( tl.at( texIndex ) );
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::D_TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            // XXX not in Core //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE2 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( tl.at( texIndex ) );
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::D_TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            // XXX not in Core //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( GL_TEXTURE1 );
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( tl.at( texIndex ) );
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            index = model->index( tl.at( texIndex ),  (int)Fn::Property::D_TEXTURE_GLUINT );
            GLuint tex = static_cast< GLuint >( model->data( index, Qt::DisplayRole ).toInt() );
            // XXX not in Core //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            glActiveTexture( /*GL_TEXTURE0*/GL_TEXTURE15 ); // XXX test sampler validation error
            glBindTexture( GL_TEXTURE_3D, tex );
            setTexInterpolation( tl.at( texIndex ) );
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        }
        default:
            break;
    }
    return true;
}

void GLFunctions::setTexInterpolation( int row )
{
    QAbstractItemModel* model = Models::d();
    QModelIndex index = model->index( row,  (int)Fn::Property::D_INTERPOLATION );
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
    Q_ASSERT_X( m_shaders[ name ] != NULL, "GLFunctions::getShader", "shader not set" );
    return m_shaders[ name ];
}

bool GLFunctions::validateShader( QString name )
{
    qDebug() << "Validating shader program" << name;
    QGLShaderProgram *program = m_shaders[ name ];
    const QString log(program->log());
    if (! log.isEmpty()) {
        qDebug()  << "log:" << log;
    }

    glValidateProgram(program->programId());
    GLint status;
    glGetProgramiv(program->programId(), GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        qCritical() << "Error validating shader program:" << name << "!";
        QByteArray buf(1024, 0);
        GLsizei length = 0;

        glGetProgramInfoLog(program->programId(), buf.length(), &length, (char *)buf.data());
        if (length > 0)
        {
            qDebug() << "program info log:" << buf;
        }
        return false;
    }
    else
    {
        qDebug() << "Shader program validated successfully: " << name;
        return true;
    }
}

std::vector< QString > GLFunctions::getShaderNames()
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
    QHash< QString, QGLShaderProgram* >::iterator iter;
    for ( iter = m_shaders.begin(); iter != m_shaders.end(); ++iter )
    {
        delete iter.value();
    }
    m_shaders.clear();
    updateColormapShader();
    for ( unsigned int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
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
        GLFunctions::m_shaderNames.push_back( "line" );
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
        GLFunctions::m_shaderNames.push_back( "diffpoints" );

        for ( unsigned int i = 0; i < GLFunctions::m_shaderNames.size(); ++i )
        {
            GLFunctions::m_shaderSources[ GLFunctions::m_shaderNames[ i ] + "_vs" ] = copyShaderToString( GLFunctions::m_shaderNames[ i ], QString("vs") );
            GLFunctions::m_shaderSources[ GLFunctions::m_shaderNames[ i ] + "_fs" ] = copyShaderToString( GLFunctions::m_shaderNames[ i ], QString("fs") );
            GLFunctions::m_shaders[ GLFunctions::m_shaderNames[ i ] ] = initShader( GLFunctions::m_shaderNames[ i ] );
            // Validate shader XXX too early, must be done after shader setup
            //validateShader( GLFunctions::m_shaderNames[ i ] );

        }

        GLFunctions::shadersLoaded = true;
    }
}

void GLFunctions::updateColormapShader()
{
    QString code( "" );

    code += "\n";
    code += "float unpackFloat( const vec4 value )\n";
    code += "{\n";
    code += "    const vec4 bitSh = vec4( 1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0 );\n";
    code += "    return ( dot( value, bitSh ) );\n";
    code += "}\n";

    int numColormaps = ColormapFunctions::size();

    code += "vec4 colormap( float value, int cmap, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax ) \n";
    code += "{ \n";
    code += "    vec3 color = vec3(0.0); \n";
    code += "    if ( value < lowerThreshold ) \n";
    code += "    { \n";
    code += "        return vec4( color, 1.0 ); \n";
    code += "    } \n";
    code += "    if ( value > upperThreshold ) \n";
    code += "    { \n";
    code += "        return vec4( color, 1.0 ); \n";
    code += "    } \n";
    code += "    value = ( value - selectedMin ) / ( selectedMax - selectedMin ); \n";
    for ( int i = 0; i < numColormaps; ++i )
    {
        code += "    if ( cmap == " + QString::number( i ) + " ) \n";
        code += "    { \n";
        code += ColormapFunctions::get( i ).getCode();
        code += "    } \n";
    }
    code += "    return vec4( color, 1.0 ); \n";
    code += "} \n";

    GLFunctions::m_shaderIncludes[ "colormap_fs" ] = code;
    //qDebug() << code;
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
#if defined(Q_OS_MAC) && QT_VERSION <= 0x040806 && QT_VERSION >= 0x040800    // if less or equal to 4.8.6
    QGLShaderProgram* program = new BugfixGLShaderProgram;
#else
    QGLShaderProgram* program = new QGLShaderProgram;
#endif

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

    const QString code_vs(code);    // XXX
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
        qDebug() << code;
        //exit( false );
    }

    // Linking shader pipeline
    if ( !program->link() )
    {
        qCritical() << "Error while linking shader: " << name << "!";
        qDebug() << "vertex shader:" << code_vs << "fragment shader:" << code;
        //exit( false );
    }

    // Binding shader pipeline for use
    if ( !program->bind() )
    {
        qCritical() << "Error while binding shader: " << name << "!";
        qDebug() << code;
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
    program->setUniformValue( "texture0", /*0*/ 15 );   // XXX Samplers of different types use the same texture image unit. - or - A sampler's texture unit is out of range (greater than max allowed or negative).
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

    float nx, ny, nz, dx, dy, dz, ndx, ndy, ndz, ax, ay, az;

    switch ( tl.size() )
    {
        case 5:
        {
            texIndex = 4;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = &ds->properties( target );
            if ( props->get( Fn::Property::D_LOCK_PROPS ).toBool() )
            {
                props = &ds->properties( "maingl" );
            }

            dx = props->get( Fn::Property::D_DX ).toFloat();
            dy = props->get( Fn::Property::D_DY ).toFloat();
            dz = props->get( Fn::Property::D_DZ ).toFloat();
            nx = props->get( Fn::Property::D_NX ).toFloat();
            ny = props->get( Fn::Property::D_NY ).toFloat();
            nz = props->get( Fn::Property::D_NZ ).toFloat();
            ndx = nx * dx;
            ndy = ny * dy;
            ndz = nz * dz;
            ax = ( props->get( Fn::Property::D_ADJUST_X ).toFloat() - dx / 2 ) / ndx;
            ay = ( props->get( Fn::Property::D_ADJUST_Y ).toFloat() - dy / 2 ) / ndy;
            az = ( props->get( Fn::Property::D_ADJUST_Z ).toFloat() - dz / 2 ) / ndz;
            program->setUniformValue( "u_dims4", QVector3D( ndx, ndy, ndz ) );
            program->setUniformValue( "u_adjust4", QVector3D( ax, ay, az ) );

            texMin = props->get( Fn::Property::D_MIN ).toFloat();
            texMax = props->get( Fn::Property::D_MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold4", ( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold4", ( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin4", ( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax4", ( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_alpha4", props->get( Fn::Property::D_ALPHA ).toFloat() );
            program->setUniformValue( "u_colormap4", props->get( Fn::Property::D_COLORMAP ).toInt() );
            program->setUniformValue( "u_texActive4", true );
        }
            /* no break */
        case 4:
        {
            texIndex = 3;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = &ds->properties( target );
            if ( props->get( Fn::Property::D_LOCK_PROPS ).toBool() )
            {
                props = &ds->properties( "maingl" );
            }
            dx = props->get( Fn::Property::D_DX ).toFloat();
            dy = props->get( Fn::Property::D_DY ).toFloat();
            dz = props->get( Fn::Property::D_DZ ).toFloat();
            nx = props->get( Fn::Property::D_NX ).toFloat();
            ny = props->get( Fn::Property::D_NY ).toFloat();
            nz = props->get( Fn::Property::D_NZ ).toFloat();
            ndx = nx * dx;
            ndy = ny * dy;
            ndz = nz * dz;
            ax = ( props->get( Fn::Property::D_ADJUST_X ).toFloat() - dx / 2 ) / ndx;
            ay = ( props->get( Fn::Property::D_ADJUST_Y ).toFloat() - dy / 2 ) / ndy;
            az = ( props->get( Fn::Property::D_ADJUST_Z ).toFloat() - dz / 2 ) / ndz;
            program->setUniformValue( "u_dims3", QVector3D( ndx, ndy, ndz ) );
            program->setUniformValue( "u_adjust3", QVector3D( ax, ay, az ) );

            texMin = props->get( Fn::Property::D_MIN ).toFloat();
            texMax = props->get( Fn::Property::D_MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold3", ( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold3", ( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin3", ( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax3", ( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_alpha3", props->get( Fn::Property::D_ALPHA ).toFloat() );
            program->setUniformValue( "u_colormap3", props->get( Fn::Property::D_COLORMAP ).toInt() );
            program->setUniformValue( "u_texActive3", true );
        }
            /* no break */
        case 3:
        {
            texIndex = 2;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = &ds->properties( target );
            if ( props->get( Fn::Property::D_LOCK_PROPS ).toBool() )
            {
                props = &ds->properties( "maingl" );
            }
            dx = props->get( Fn::Property::D_DX ).toFloat();
            dy = props->get( Fn::Property::D_DY ).toFloat();
            dz = props->get( Fn::Property::D_DZ ).toFloat();
            nx = props->get( Fn::Property::D_NX ).toFloat();
            ny = props->get( Fn::Property::D_NY ).toFloat();
            nz = props->get( Fn::Property::D_NZ ).toFloat();
            ndx = nx * dx;
            ndy = ny * dy;
            ndz = nz * dz;
            ax = ( props->get( Fn::Property::D_ADJUST_X ).toFloat() - dx / 2 ) / ndx;
            ay = ( props->get( Fn::Property::D_ADJUST_Y ).toFloat() - dy / 2 ) / ndy;
            az = ( props->get( Fn::Property::D_ADJUST_Z ).toFloat() - dz / 2 ) / ndz;
            program->setUniformValue( "u_dims2", QVector3D( ndx, ndy, ndz ) );
            program->setUniformValue( "u_adjust2", QVector3D( ax, ay, az ) );

            texMin = props->get( Fn::Property::D_MIN ).toFloat();
            texMax = props->get( Fn::Property::D_MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold2", ( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold2", ( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin2", ( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax2", ( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_alpha2", props->get( Fn::Property::D_ALPHA ).toFloat() );
            program->setUniformValue( "u_colormap2", props->get( Fn::Property::D_COLORMAP ).toInt() );
            program->setUniformValue( "u_texActive2", true );
        }
            /* no break */
        case 2:
        {
            texIndex = 1;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = &ds->properties( target );
            if ( props->get( Fn::Property::D_LOCK_PROPS ).toBool() )
            {
                props = &ds->properties( "maingl" );
            }
            dx = props->get( Fn::Property::D_DX ).toFloat();
            dy = props->get( Fn::Property::D_DY ).toFloat();
            dz = props->get( Fn::Property::D_DZ ).toFloat();
            nx = props->get( Fn::Property::D_NX ).toFloat();
            ny = props->get( Fn::Property::D_NY ).toFloat();
            nz = props->get( Fn::Property::D_NZ ).toFloat();
            ndx = nx * dx;
            ndy = ny * dy;
            ndz = nz * dz;
            ax = ( props->get( Fn::Property::D_ADJUST_X ).toFloat() - dx / 2 ) / ndx;
            ay = ( props->get( Fn::Property::D_ADJUST_Y ).toFloat() - dy / 2 ) / ndy;
            az = ( props->get( Fn::Property::D_ADJUST_Z ).toFloat() - dz / 2 ) / ndz;
            program->setUniformValue( "u_dims1", QVector3D( ndx, ndy, ndz ) );
            program->setUniformValue( "u_adjust1", QVector3D( ax, ay, az ) );

            texMin = props->get( Fn::Property::D_MIN ).toFloat();
            texMax = props->get( Fn::Property::D_MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold1", ( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold1", ( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin1", ( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax1", ( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_alpha1", props->get( Fn::Property::D_ALPHA ).toFloat() );
            program->setUniformValue( "u_colormap1", props->get( Fn::Property::D_COLORMAP ).toInt() );
            program->setUniformValue( "u_texActive1", true );

        }
            /* no break */
        case 1:
        {
            texIndex = 0;
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( tl.at( texIndex ), (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            PropertyGroup* props = &ds->properties( target );
            if ( props->get( Fn::Property::D_LOCK_PROPS ).toBool() )
            {
                props = &ds->properties( "maingl" );
            }
            dx = props->get( Fn::Property::D_DX ).toFloat();
            dy = props->get( Fn::Property::D_DY ).toFloat();
            dz = props->get( Fn::Property::D_DZ ).toFloat();
            nx = props->get( Fn::Property::D_NX ).toFloat();
            ny = props->get( Fn::Property::D_NY ).toFloat();
            nz = props->get( Fn::Property::D_NZ ).toFloat();
            ndx = nx * dx;
            ndy = ny * dy;
            ndz = nz * dz;
            ax = ( props->get( Fn::Property::D_ADJUST_X ).toFloat() - dx / 2 ) / ndx;
            ay = ( props->get( Fn::Property::D_ADJUST_Y ).toFloat() - dy / 2 ) / ndy;
            az = ( props->get( Fn::Property::D_ADJUST_Z ).toFloat() - dz / 2 ) / ndz;
            program->setUniformValue( "u_dims0", QVector3D( ndx, ndy, ndz ) );
            program->setUniformValue( "u_adjust0", QVector3D( ax, ay, az ) );

            texMin = props->get( Fn::Property::D_MIN ).toFloat();
            texMax = props->get( Fn::Property::D_MAX ).toFloat();
            program->setUniformValue( "u_lowerThreshold0", ( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_upperThreshold0", ( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMin0", ( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_selectedMax0", ( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
            program->setUniformValue( "u_alpha0", props->get( Fn::Property::D_ALPHA ).toFloat() );
            program->setUniformValue( "u_colormap0", props->get( Fn::Property::D_COLORMAP ).toInt() );
            GLFunctions::sliceAlpha[target] = props->get( Fn::Property::D_ALPHA ).toFloat();
            program->setUniformValue( "u_texActive0", true );
        }
            break;
        default:
            break;
    }
    GLFunctions::getAndPrintGLError( target + " set shader vars #2" );
}

QList< int > GLFunctions::getTextureIndexes( QString target )
{
    QList< int > tl;
    QAbstractItemModel* model = Models::d();
    int countDatasets = model->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        PropertyGroup* props = &ds->properties( target );
        bool active = props->get( Fn::Property::D_ACTIVE ).toBool();
        bool isTex = props->get( Fn::Property::D_HAS_TEXTURE ).toBool();

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

bool GLFunctions::getAndPrintGLError( QString prefix )
{
    GLenum errCode;
    const char *errString;
    bool isError = false;
    while ( 0 && /*XXXXX disabled*/ ( errCode = glGetError() ) != GL_NO_ERROR )
    {
        switch(errCode)
        {
            case GL_INVALID_OPERATION:              errString="INVALID_OPERATION";              break;
            case GL_INVALID_ENUM:                   errString="INVALID_ENUM";                   break;
            case GL_INVALID_VALUE:                  errString="INVALID_VALUE";                  break;
            case GL_OUT_OF_MEMORY:                  errString="OUT_OF_MEMORY";                  break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  errString="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        qDebug() << "OpenGL Error:" << prefix << QString( errString );
        isError = true;
    }
    return isError;
}

void GLFunctions::deleteTexture( GLuint tex )
{
    m_texturesToDelete.push_back( tex );
}
