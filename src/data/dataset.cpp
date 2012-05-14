/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QStringList>

#include "dataset.h"

Dataset::Dataset( QDir fileName, FN_DATASET_TYPE type )
{
    m_properties["fileName"] = fileName.path();
    m_properties["type"] = type;
    m_properties["name"] = fileName.path().split("/").last();
    m_properties["size"] = -1;
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
