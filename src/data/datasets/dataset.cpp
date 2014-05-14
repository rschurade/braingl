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
    PropertyGroup props;
    // add standard properties
    props.createBool( Fn::Property::D_ACTIVE, true );
    props.createBool( Fn::Property::D_LOCK_PROPS, false );
    props.createDir( Fn::Property::D_FILENAME, fileName.path() );
    props.createInt( Fn::Property::D_TYPE, (int)type );
    props.createString( Fn::Property::D_NAME, fileName.path().split( "/" ).last(), "general" );
    props.createInt( Fn::Property::D_SIZE, -1 );
    props.createInt( Fn::Property::D_CREATED_BY, (int)Fn::Algo::NONE );

    props.createBool( Fn::Property::D_HAS_TEXTURE, false );
    props.createString( Fn::Property::D_RENDER_TARGET, "maingl" );

    m_properties.insert( "maingl", props );
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

PropertyGroup& Dataset::properties( QString target )
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

bool Dataset::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    // do nothing here
    return false;
}

bool Dataset::rightMouseDrag( int pickId, QVector3D dir, Qt::KeyboardModifiers modifiers, QString target )
{
    // do nothing here
    return false;
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

void Dataset::copySettings( QString target )
{
    PropertyGroup props( ( m_properties["maingl"] ) );
    m_properties.insert( target, props );
    m_properties[target].getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
    m_properties[target].getProperty( Fn::Property::D_LOCK_PROPS )->setPropertyTab( "general" );
}

void Dataset::copyPropertyObject( PropertyGroup& props, QString target )
{
    if ( m_properties.contains( target ) )
    {
        m_properties.remove( target );
    }
    m_properties.insert( target, props );
}

QString Dataset::getSaveFilter()
{
    return QString( "" );
}

QString Dataset::getDefaultSuffix()
{
    return QString( "" );
}
