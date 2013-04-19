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
    // add standard properties
    m_properties.set( Fn::Property::ACTIVE, true );
    m_properties.set( Fn::Property::FILENAME, fileName.path() );
    m_properties.set( Fn::Property::TYPE, (int)type );
    m_properties.set( Fn::Property::NAME, fileName.path().split( "/" ).last(), true );
    m_properties.set( Fn::Property::SIZE, -1 );
    m_properties.set( Fn::Property::CREATED_BY, (int)Fn::Algo::NONE );

    m_properties.set( Fn::Property::HAS_TEXTURE, false );
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

PropertyGroup* Dataset::properties()
{
    return &m_properties;
}

void Dataset::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height )
{
    // do nothing here
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
