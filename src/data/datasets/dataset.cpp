/*
 * dataset.cpp
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */
#include "dataset.h"

Dataset::Dataset( QDir fileName, Fn::DatasetType type ) :
    m_textureGLuint( 0 )
{
    PropertyGroup* props = new PropertyGroup();
    // add standard properties
    props->set( Fn::Property::D_ACTIVE, true );
    props->set( Fn::Property::D_FILENAME, fileName.path() );
    props->set( Fn::Property::D_TYPE, (int)type );
    props->set( Fn::Property::D_NAME, fileName.path().split( "/" ).last(), "general" );
    props->set( Fn::Property::D_SIZE, -1 );
    props->set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::NONE );

    props->set( Fn::Property::D_HAS_TEXTURE, false );
    props->set( Fn::Property::D_RENDER_TARGET, "maingl" );

    m_properties.insert( "maingl", props );

    PropertyGroup* props2 = new PropertyGroup();
    // add standard properties
    props2->set( Fn::Property::D_ACTIVE, true, "general" );
    props2->set( Fn::Property::D_FILENAME, fileName.path() );
    props2->set( Fn::Property::D_TYPE, (int)type );
    props2->set( Fn::Property::D_RENDER_TARGET, "maingl2" );
    m_properties.insert( "maingl2", props2 );
}

Dataset::~Dataset()
{
}

GLuint Dataset::getTextureGLuint()
{
    if ( m_textureGLuint == 0 )
    {
        createTexture();
    }
    return m_textureGLuint;
}

PropertyGroup* Dataset::properties( QString target )
{
    if ( m_properties.contains( target ) )
    {
        return m_properties[target];
    }
    else
    {
        return m_properties["maingl"];
    }
}

QString Dataset::getValueAsString( int x, int y, int z )
{
    return QString( "0" );
}


void Dataset::createTexture()
{
    // do nothing here
}

void Dataset::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers )
{
    // do nothing here
}

QString Dataset::getColormapShader( int num )
{
    QString code( "" );

    code += "vec4 colormap" + QString::number( num ) + "( vec4 color, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax ) \n";
    code += "{ \n";
    code += "    return vec4( 0.0 ); \n";
    code += "} \n";

    return code;
}
