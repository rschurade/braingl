/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include "dataset.h"

Dataset::Dataset( QDir fileName, Fn::DatasetType type ) :
    m_textureGLuint( 0 )
{
    // add standard properties
    m_properties.set( FNPROP_ACTIVE, true, true );
    m_properties.set( FNPROP_FILENAME, fileName.path() );
    m_properties.set( FNPROP_TYPE, (int)type );
    m_properties.set( FNPROP_NAME, fileName.path().split( "/" ).last(), true );
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

void Dataset::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model )
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
