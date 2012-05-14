/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QStringList>

#include "dataset.h"

Dataset::Dataset( QDir fileName, FN_DATASET_TYPE type ) :
	m_fileName( fileName ),
	m_type( type ),
	m_size( -1 )
{
    m_name = m_fileName.path().split("/").last();
}

Dataset::~Dataset()
{
}

QDir Dataset::getFilename()
{
	return m_fileName;
}

FN_DATASET_TYPE Dataset::getType()
{
	return m_type;
}

QString Dataset::getName()
{
	return m_name;
}

void Dataset::setName( QString name )
{
    m_name = name;
}

int Dataset::getSize()
{
    return m_size;
}
