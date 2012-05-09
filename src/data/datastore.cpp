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
}

DataStore::~DataStore()
{
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

void DataStore::moveItemUp( int row )
{
	beginMoveRows( index( row, 0 ), row, row, index( row - 1, 0 ), row - 1 );
	m_datasetList.swap( row, row - 1 );
	endMoveRows();
}

void DataStore::moveItemDown( int row )
{
	beginMoveRows( index( row, 0 ), row, row, index( row + 1, 0 ), row + 1 );
	m_datasetList.swap( row, row + 1 );
	endMoveRows();
}

void DataStore::deleteItem( int row )
{
    if ( row >= 0 && row < m_datasetList.size() )
    {
        beginRemoveRows( index( row, 0 ), row, row );
        m_datasetList.removeAt( row );
        endRemoveRows();
        beginResetModel();
        reset();
        endResetModel();
    }
}
