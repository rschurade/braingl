/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QStringList>

#include "dataset.h"

Dataset::Dataset( QDir fileName, FN_DATASET_TYPE type ) :
    m_textureGLuint( 0 )
{
    // add standard properties
    m_properties.set( FNPROP_ACTIVE, true );
    m_properties.set( FNPROP_FILENAME, fileName.path() );
    m_properties.set( FNPROP_TYPE, type );
    m_properties.set( FNPROP_NAME, fileName.path().split( "/" ).last() );
    m_properties.set( FNPROP_SIZE, -1 );
    m_properties.set( FNPROP_CREATED_BY, FNALGO_NONE );

    m_properties.set( FNPROP_RENDER_LOWER_X, 0 );
    m_properties.set( FNPROP_RENDER_LOWER_Y, 0 );
    m_properties.set( FNPROP_RENDER_LOWER_Z, 0 );
    m_properties.set( FNPROP_RENDER_UPPER_X, 0 );
    m_properties.set( FNPROP_RENDER_UPPER_Y, 0 );
    m_properties.set( FNPROP_RENDER_UPPER_Z, 0 );
}

Dataset::~Dataset()
{
}

void Dataset::setProperty( FN_PROPERTY name, bool value )
{
    m_properties.set( name, value );
}

void Dataset::setProperty( FN_PROPERTY name, int value )
{
    m_properties.set( name, value );
}

void Dataset::setProperty( FN_PROPERTY name, float value )
{
    m_properties.set( name, value );
}

void Dataset::setProperty( FN_PROPERTY name, QString value )
{
    m_properties.set( name, value );
}


QVariant Dataset::getProperty( FN_PROPERTY name )
{
    if ( m_properties.contains( name ) )
    {
        return m_properties.getValue( name );
    }
    return QVariant();
}

GLuint Dataset::getTextureGLuint()
{
    if ( m_textureGLuint == 0 )
    {
        createTexture();
    }
    return m_textureGLuint;
}

PropertyGroup Dataset::getProperties()
{
    return m_properties;
}

void Dataset::setProperties( PropertyGroup props )
{
    m_properties = props;
}
