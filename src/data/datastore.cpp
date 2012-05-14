/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <QtCore/QLocale>
#include <QtCore/QDebug>

#include "datasetscalar.h"
#include "loader.h"

#include "datastore.h"

DataStore::DataStore()
{
    m_globals[ "axial" ] = 80;
    m_globals[ "coronal" ] = 100;
    m_globals[ "sagittal" ] = 80;
    m_globals[ "max_axial" ] = 160;
    m_globals[ "max_coronal" ] = 200;
    m_globals[ "max_sagittal" ] = 160;
    m_globals[ "min_axial" ] = 0;
    m_globals[ "min_coronal" ] = 0;
    m_globals[ "min_sagittal" ] = 0;
    m_globals[ "slice_dx" ] = 1.0;
    m_globals[ "slice_dy" ] = 1.0;
    m_globals[ "slice_dz" ] = 1.0;
}

DataStore::~DataStore()
{
}

void DataStore::addDataset( Dataset* dataset )
{
    beginInsertRows( QModelIndex(), m_datasetList.size(), m_datasetList.size() );
    m_datasetList.push_back( dataset );
    endInsertRows();

    // TODO dont just use the first dataset
    // check if there is maybe first texture added
    if ( m_datasetList.size() == 1 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->getProperty( "type" ).toInt() == FNDT_NIFTI_SCALAR )
        {
            m_globals[ "axial" ] = ds->getProperty( "nz" ).toInt() / 2;
            m_globals[ "coronal" ] = ds->getProperty( "ny" ).toInt() / 2;
            m_globals[ "sagittal" ] = ds->getProperty( "nx" ).toInt() / 2;
        }
    }
    updateGlobals();

    emit ( datasetListChanged() );
}

bool DataStore::load( QDir fileName )
{
    Loader loader( fileName );
    if ( loader.load() )
    {
        addDataset( loader.getDataset() );
    }
    return loader.succes();
}

int DataStore::rowCount( const QModelIndex &parent ) const
{
    return m_datasetList.count();
}

int DataStore::columnCount( const QModelIndex &parent ) const
{
    return 10;
}

QVariant DataStore::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= m_datasetList.size() )
        return QVariant();

    if ( role == Qt::DisplayRole && index.column() == 0 )
    {
        return m_datasetList.at( index.row() )->getProperty( "name" ).toString();
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

bool DataStore::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( !index.isValid() )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        if ( index.row() >= 0 && index.row() < m_datasetList.size() )
        {
            if ( index.column() == 0 )
            {
                m_datasetList.at( index.row() )->setProperty( "name", value.toString() );
            }
        }
        emit( dataChanged( index, index ) );
        emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
        return true;
    }
    return false;
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
                return QString( "name" );
                break;
            case 1:
                return QString( "dim" );
                break;
            case 2:
                return QString( "data type" );
                break;
            case 3:
                return QString( "size in byte" );
                break;
            case 4:
                return QString( "nx" );
                break;
            case 5:
                return QString( "ny" );
                break;
            case 6:
                return QString( "nz" );
                break;
            case 7:
                return QString( "dx" );
                break;
            case 8:
                return QString( "dy" );
                break;
            case 9:
                return QString( "dz" );
                break;
        }
    }
    else
    {
        return m_datasetList.at( section )->getProperty( "name" ).toString();
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
    updateGlobals();
    emit ( datasetListChanged() );
}

void DataStore::moveItemDown( int row )
{
    beginMoveRows( index( row, 0 ), row, row, index( row + 1, 0 ), row + 1 );
    m_datasetList.swap( row, row + 1 );
    endMoveRows();
    updateGlobals();
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
        updateGlobals();
        emit ( datasetListChanged() );
    }
}

QVariant DataStore::datasetInfo( const QModelIndex &index ) const
{
    FN_DATASET_TYPE type = static_cast< FN_DATASET_TYPE >( m_datasetList.at( index.row() )->getProperty( "type" ).toInt() );

    if ( type == FNDT_NIFTI_SCALAR || type == FNDT_NIFTI_VECTOR )
    {
        DatasetNifti* ds = dynamic_cast< DatasetNifti* >( m_datasetList.at( index.row() ) );

        switch ( index.column() )
        {
            case 1:
                return ds->getProperty( "nt" ).toInt();
                break;
            case 2:
                return getNiftiDataType( ds->getProperty( "datatype" ).toInt() );
                break;
            case 3:
                QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );
                return QString( "%L1" ).arg( ds->getProperty( "size" ).toInt() );
                break;
            case 4:
                return ds->getProperty( "nx" ).toInt();
                break;
            case 5:
                return ds->getProperty( "ny" ).toInt();
                break;
            case 6:
                return ds->getProperty( "nz" ).toInt();
                break;
            case 7:
                return ds->getProperty( "dx" ).toFloat();
                break;
            case 8:
                return ds->getProperty( "dy" ).toFloat();
                break;
            case 9:
                return ds->getProperty( "dz" ).toFloat();
                break;
            default:
                break;
        }
    }
    return QVariant();
}

QString DataStore::getNiftiDataType( const int type ) const
{
    switch ( type )
    {
        case 0:
            return QString( "unknown" );
            break;
        case 1:
            return QString( "binary" );
            break;
        case 2:
            return QString( "unsigned char" );
            break;
        case 4:
            return QString( "signed short" );
            break;
        case 8:
            return QString( "signed int" );
            break;
        case 16:
            return QString( "float" );
            break;
        case 32:
            return QString( "complex" );
            break;
        case 64:
            return QString( "double" );
            break;
        case 128:
            return QString( "RGB" );
            break;
        default:
            return QString( "unknown" );
            break;
    }
    return QString( "unknown" );
}

GLuint DataStore::getFirstTexture()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->getProperty( "type" ).toInt() == FNDT_NIFTI_SCALAR )
        {
            return dynamic_cast< DatasetScalar* >( ds )->getTextureGLuint();
        }

    }
    return 0;
}

void DataStore::setGlobal( QString key, QVariant data )
{
    //qDebug() << "set global: " << key << "," << data;
    m_globals[ key ] = data;
    emit( globalSettingChanged( key, data ) );
}

QVariant DataStore::getGlobalSetting( QString name )
{
    if ( m_globals.contains( name ) )
    {
        return m_globals[ name ];
    }
    return QVariant();
}

void DataStore::updateGlobals()
{
    updateSliceGlobals();
}

void DataStore::updateSliceGlobals()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->getProperty( "type" ).toInt() == FNDT_NIFTI_SCALAR )
        {
            m_globals[ "max_axial" ] = ds->getProperty( "nz" ).toInt();
            m_globals[ "max_coronal" ] = ds->getProperty( "ny" ).toInt();
            m_globals[ "max_sagittal" ] = ds->getProperty( "nx" ).toInt();
            m_globals[ "slice_dx" ] = ds->getProperty( "dx" ).toFloat();
            m_globals[ "slice_dy" ] = ds->getProperty( "dy" ).toFloat();
            m_globals[ "slice_dz" ] = ds->getProperty( "dz" ).toFloat();

            emit( globalSettingChanged( "max_axial", ds->getProperty( "nz").toInt() ) );
            emit( globalSettingChanged( "max_coronal", ds->getProperty( "ny").toInt() ) );
            emit( globalSettingChanged( "max_sagittal", ds->getProperty( "nx").toInt() ) );

            emit( globalSettingChanged( "axial", m_globals[ "axial" ] ) );
            emit( globalSettingChanged( "coronal", m_globals[ "coronal" ] ) );
            emit( globalSettingChanged( "sagittal", m_globals[ "sagittal" ] ) );
        }
    }
}
