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
    props->set( Fn::Property::ACTIVE, true );
    props->set( Fn::Property::FILENAME, fileName.path() );
    props->set( Fn::Property::TYPE, (int)type );
    props->set( Fn::Property::NAME, fileName.path().split( "/" ).last(), true );
    props->set( Fn::Property::SIZE, -1 );
    props->set( Fn::Property::CREATED_BY, (int)Fn::Algo::NONE );

    props->set( Fn::Property::HAS_TEXTURE, false );
    props->set( Fn::Property::RENDER_TARGET, "maingl" );

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

void Dataset::mousePick( int pickId, QVector3D pos )
{
    // do nothing here
}
