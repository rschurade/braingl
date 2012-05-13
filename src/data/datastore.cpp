/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <QtCore/QLocale>

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

    emit ( datasetListChanged() );
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
	emit ( datasetListChanged() );

	return loader.succes();
}

int DataStore::rowCount( const QModelIndex &parent ) const
{
	return m_datasetList.count();
}

int DataStore::columnCount( const QModelIndex &parent ) const
{
	return 7;
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
	else if ( role == Qt::DisplayRole && index.column() > 0 )
	{
	    return datasetInfo( index );
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
	{
	    switch ( section )
	    {
	        case 0:
	            return QString("name");
	            break;
	        case 1:
                return QString("dim");
                break;
	        case 2:
                return QString("data type");
                break;
	        case 3:
                return QString("nx");
                break;
            case 4:
                return QString("ny");
                break;
            case 5:
                return QString("nz");
                break;
            case 6:
                return QString("size in byte");
                break;
	    }
	}
	else
	{
		return m_datasetList.at( section )->getShortFilename();
	}
	return QVariant();
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
	emit ( datasetListChanged() );
}

void DataStore::moveItemDown( int row )
{
	beginMoveRows( index( row, 0 ), row, row, index( row + 1, 0 ), row + 1 );
	m_datasetList.swap( row, row + 1 );
	endMoveRows();
	emit ( datasetListChanged() );
}

void DataStore::deleteItem( int row )
{
    if ( row >= 0 && row < m_datasetList.size() )
    {
        beginRemoveRows( index( row, 0 ), row, row );
        Dataset* toDelete = m_datasetList.at( row );
        m_datasetList.removeAt( row );
        endRemoveRows();
        beginResetModel();
        reset();
        endResetModel();

        delete toDelete;

        emit ( datasetListChanged() );
    }
}

QVariant DataStore::datasetInfo( const QModelIndex &index ) const
{
    FN_DATASET_TYPE type = m_datasetList.at( index.row() )->getType();


    if ( type == FNDT_NIFTI_SCALAR || type == FNDT_NIFTI_VECTOR )
    {
        DatasetNifti* ds = dynamic_cast<DatasetNifti*>( m_datasetList.at( index.row() ) );

        switch ( index.column() )
        {
            case 1:
                return ds->getNt();
                break;
            case 2:
                return getNiftiDataType( ds->getDatatype() );
                break;
            case 3:
                return ds->getNx();
                break;
            case 4:
                return ds->getNy();
                break;
            case 5:
                return ds->getNz();
                break;
            case 6:
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                return QString("%L1").arg( ds->getSize() );
                break;
            default:
                break;
        }
    }
    return QVariant();
}

QString DataStore::getNiftiDataType( const int type ) const
{
    switch( type )
    {
        case 0:
            return QString("unknown");
            break;
        case 1:
            return QString("binary");
            break;
        case 2:
            return QString("unsigned char");
            break;
        case 4:
            return QString("signed short");
            break;
        case 8:
            return QString("signed int");
            break;
        case 16:
            return QString("float");
            break;
        case 32:
            return QString("complex");
            break;
        case 64:
            return QString("double");
            break;
        case 128:
            return QString("RGB");
            break;
        default:
            return QString("unknown");
            break;
    }
    return QString("unknown");
}

GLuint DataStore::getFirstTexture()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->getType() == FNDT_NIFTI_SCALAR )
        {
            return dynamic_cast<DatasetScalar*>( ds )->getTextureGLuint();
        }

    }
    return 0;
}
