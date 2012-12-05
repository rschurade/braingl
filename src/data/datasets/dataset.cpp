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
    m_properties["fileName"] = fileName.path();
    m_properties["type"] = type;
    m_properties["name"] = fileName.path().split( "/" ).last();
    m_properties["size"] = -1;
    m_properties["createdBy"] = FNALGO_NONE;

    m_properties["renderLowerX"] = 0;
    m_properties["renderLowerY"] = 0;
    m_properties["renderLowerZ"] = 0;
    m_properties["renderUpperX"] = 0;
    m_properties["renderUpperY"] = 0;
    m_properties["renderUpperZ"] = 0;
}

Dataset::~Dataset()
{
}

QVariant Dataset::getProperty( QString name )
{
    if ( m_properties.contains( name ) )
    {
        return m_properties[name];
    }
    return QVariant();
}

void Dataset::setProperty( QString name, QVariant value )
{
    m_properties[name] = value;
}

GLuint Dataset::getTextureGLuint()
{
    if ( m_textureGLuint == 0 )
    {
        createTexture();
    }
    return m_textureGLuint;
}

QHash<QString, QVariant> Dataset::getProperties()
{
    return m_properties;
}

void Dataset::setProperties( QHash<QString, QVariant> props )
{
    m_properties = props;
}
