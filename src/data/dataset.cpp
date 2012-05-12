/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QStringList>

#include "dataset.h"

Dataset::Dataset( QString fileName, FN_DATASET_TYPE type ) :
	m_fileName( fileName ),
	m_type( type ),
	m_size( -1 )
{
}

Dataset::~Dataset()
{
}

QString Dataset::getFilename()
{
	return m_fileName;
}

FN_DATASET_TYPE Dataset::getType()
{
	return m_type;
}

QString Dataset::getShortFilename()
{
	return m_fileName.split("/").last();
}

int Dataset::getSize()
{
    return m_size;
}
