/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include "datasetscalar.h"
#include "loader.h"

#include "datastore.h"

DataStore::DataStore()
{
    // TODO Auto-generated constructor stub

}

DataStore::~DataStore()
{
    // TODO Auto-generated destructor stub
}

void DataStore::addDataset( Dataset* dataset )
{
    m_datasetList.push_back( dataset );
}

bool DataStore::load( QString fileName )
{
	Loader loader( fileName );
	if ( loader.load() )
	{
		beginInsertRows(QModelIndex(), m_datasetList.size(), m_datasetList.size());
		m_datasetList.push_back( loader.getDataset() );
		endInsertRows();
	}
	return loader.succes();
}

int DataStore::rowCount( const QModelIndex &parent ) const
{
	return m_datasetList.count();
}

int DataStore::columnCount( const QModelIndex &parent ) const
{
	return 3;
}


QVariant DataStore::data( const QModelIndex &index, int role ) const
{
	if ( !index.isValid() )
		return QVariant();

	if ( index.row() >= m_datasetList.size() )
		return QVariant();

	if ( role == Qt::DisplayRole && index.column() == 0 )
	{
		return m_datasetList.at( index.row() )->getShortFilename();
	}
	else if ( role == Qt::DisplayRole && index.column() == 1 )
	{
		if ( m_datasetList.at( index.row() )->getType() == FNDT_NIFTI_SCALAR )
		{
			return QString( "scalar" );
		}
	}
	else if ( role == Qt::DisplayRole && index.column() == 2 )
	{
		if ( m_datasetList.at( index.row() )->getType() == FNDT_NIFTI_SCALAR )
		{
			return dynamic_cast<DatasetScalar*>( m_datasetList.at( index.row() ) )->getDatatype();
		}
	}
	else
	{
		return QVariant();
	}
	return QVariant();
}

QVariant DataStore::headerData( int section, Qt::Orientation orientation, int role ) const
{
	if ( role != Qt::DisplayRole )
		return QVariant();

	if ( orientation == Qt::Horizontal )
		return QString( "Column %1" ).arg( section );
	else
		return QString( "Row %1" ).arg( section );
}

QModelIndex DataStore::index( int row, int column, const QModelIndex & parent ) const
{
	return createIndex( row, column, m_datasetList.at( row ) );
}

QModelIndex DataStore::parent( const QModelIndex & index ) const
{
	return QModelIndex();
}
