/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <QtCore/QLocale>
#include <QtCore/QDebug>

#include "loader.h"
#include "writer.h"
#include "vptr.h"
#include "enums.h"

#include "datastore.h"

DataStore::DataStore()
{
    m_globalProperties.set( FNPROP_NAME, QString("datastore") );
    m_globalProperties.set( FNGLOBAL_AXIAL, 0 );
    m_globalProperties.set( FNGLOBAL_CORONAL, 0 );
    m_globalProperties.set( FNGLOBAL_SAGITTAL, 0 );
    m_globalProperties.set( FNGLOBAL_MAX_AXIAL, 1 );
    m_globalProperties.set( FNGLOBAL_MAX_CORONAL, 1 );
    m_globalProperties.set( FNGLOBAL_MAX_SAGITTAL, 1 );
    m_globalProperties.set( FNGLOBAL_SLICE_DX, 1.0f );
    m_globalProperties.set( FNGLOBAL_SLICE_DY, 1.0f );
    m_globalProperties.set( FNGLOBAL_SLICE_DZ, 1.0f );
    m_globalProperties.set( FNGLOBAL_LAST_PATH, QString("") );
    m_globalProperties.set( FNGLOBAL_SHOW_AXIAL, true );
    m_globalProperties.set( FNGLOBAL_SHOW_CORONAL, true );
    m_globalProperties.set( FNGLOBAL_SHOW_SAGITTAL, true );
    m_globalProperties.set( FNGLOBAL_CORONAL_AXIAL, 0 );
    m_globalProperties.set( FNGLOBAL_SAGITTAL_AXIAL, 0 );
    m_globalProperties.set( FNGLOBAL_SAGITTAL_CORONAL, 0 );
    m_globalProperties.set( FNGLOBAL_ZOOM, 1.0f );
    m_globalProperties.set( FNGLOBAL_MOVEX, 0 );
    m_globalProperties.set( FNGLOBAL_MOVEY, 0 );
    m_globalProperties.set( FNGLOBAL_BBX, 0 );
    m_globalProperties.set( FNGLOBAL_BBY, 0 );
    m_globalProperties.set( FNGLOBAL_VIEW, 0 );
}

DataStore::~DataStore()
{
}

void DataStore::addDataset( Dataset* dataset )
{
    beginInsertRows( QModelIndex(), m_datasetList.size(), m_datasetList.size() );
    m_datasetList.push_back( VPtr<Dataset>::asQVariant( dataset ) );
    connect( dataset->properties(), SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
    endInsertRows();

    updateGlobals();

    emit ( dataChanged( index( m_datasetList.size() - 1, 0 ), index( m_datasetList.size() - 1, 0 ) ) );
}

void DataStore::updateGlobals()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( m_datasetList.first() );
        if ( ds->properties()->get( FNPROP_TYPE ).toInt() < FNDT_MESH_ASCII )
        {
            m_globalProperties.set( FNGLOBAL_AXIAL, ds->properties()->get( FNPROP_NZ ).toInt() / 2 );
            m_globalProperties.set( FNGLOBAL_CORONAL, ds->properties()->get( FNPROP_NY ).toInt() / 2 );
            m_globalProperties.set( FNGLOBAL_SAGITTAL, ds->properties()->get( FNPROP_NX ).toInt() / 2 );
            m_globalProperties.set( FNGLOBAL_MAX_AXIAL, ds->properties()->get( FNPROP_NZ ).toInt() );
            m_globalProperties.set( FNGLOBAL_MAX_CORONAL, ds->properties()->get( FNPROP_NY ).toInt() );
            m_globalProperties.set( FNGLOBAL_MAX_SAGITTAL, ds->properties()->get( FNPROP_NX ).toInt() );
            m_globalProperties.set( FNGLOBAL_SLICE_DX, ds->properties()->get( FNPROP_DX ).toFloat() );
            m_globalProperties.set( FNGLOBAL_SLICE_DY, ds->properties()->get( FNPROP_DY ).toFloat() );
            m_globalProperties.set( FNGLOBAL_SLICE_DZ, ds->properties()->get( FNPROP_DZ ).toFloat() );

            emit dataChanged( index( 0, FNGLOBAL_SAGITTAL ), index( 0, FNGLOBAL_SLICE_DZ ) );
        }
    }
}

bool DataStore::load( QDir fileName )
{
    Loader loader( fileName );
    if ( loader.load() )
    {
        for ( int i = 0; i < loader.getNumDatasets(); ++i )
        {
            addDataset( loader.getDataset( i ) );
        }
    }
    return loader.succes();
}

bool DataStore::save( int index, QString fileName )
{
    if ( index >= 0 && index < m_datasetList.size() )
    {
        Writer writer( VPtr<Dataset>::asPtr( m_datasetList[index] ), fileName );
        return writer.save();
    }
    return false;
}

int DataStore::rowCount( const QModelIndex &parent ) const
{
    return m_datasetList.size();
}

int DataStore::columnCount( const QModelIndex &parent ) const
{
    return 14;
}

QVariant DataStore::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    switch ( role )
    {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDatasetProperties( index );
            break;
        case Qt::UserRole:
            return getGlobal( index );
            break;
        default:
            break;
    }

    return QVariant();
}

QVariant DataStore::getDatasetProperties( const QModelIndex &index ) const
{
    Dataset* ds = VPtr<Dataset>::asPtr( m_datasetList.at( index.row() ) );

    // handle props with special treatment
    switch ( index.column() )
    {
        case FNPROP_TEXTURE_GLUINT:
            return ds->getTextureGLuint();
            break;
        case FNPROP_DATASET_POINTER:
            return m_datasetList.at( index.row() );
            break;
        case FNPROP_DATASET_LIST:
            return m_datasetList;
            break;
    }
    // everything else
    return ds->properties()->get( (FN_PROPERTY)index.column() );
}

QVariant DataStore::getGlobal( const QModelIndex &index ) const
{
    return m_globalProperties.get( (FN_PROPERTY)index.column() );
}

bool DataStore::setData( const QModelIndex &index, const QVariant &value, int role )
{
    //qDebug() << "row: " << index.row() << "column: " << index.column() << "role: " << role;
    if ( !index.isValid() )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        // handle props with special treatment
        if ( index.column() == FNPROP_DATASET_POINTER )
        {
            addDataset( VPtr<Dataset>::asPtr( value ) );
            emit( dataChanged( index, index ) );
            emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
            return true;
        }
        // everything else
        if ( index.row() >= 0 && index.row() < m_datasetList.size() )
        {
            VPtr<Dataset>::asPtr( m_datasetList.at( index.row() ) )->properties()->set( (FN_PROPERTY)index.column(), value );
        }
        emit( dataChanged( index, index ) );
        emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
        return true;
    }

    if ( role == Qt::UserRole )
    {
        switch( index.column() )
        {
            case FNGLOBAL_CORONAL_AXIAL:
                m_globalProperties.set( FNGLOBAL_CORONAL, value.toPoint().x() );
                m_globalProperties.set( FNGLOBAL_AXIAL, value.toPoint().y() );
                break;
            case FNGLOBAL_SAGITTAL_AXIAL:
                m_globalProperties.set( FNGLOBAL_SAGITTAL, value.toPoint().x() );
                m_globalProperties.set( FNGLOBAL_AXIAL, value.toPoint().y() );
                break;
            case FNGLOBAL_SAGITTAL_CORONAL:
                m_globalProperties.set( FNGLOBAL_SAGITTAL, value.toPoint().x() );
                m_globalProperties.set( FNGLOBAL_CORONAL, value.toPoint().y() );
                break;
        }
        m_globalProperties.set( (FN_PROPERTY)index.column(), value );
        // zoom - bby are updated in the render loop, emiting their changes causes an infinite event loop and seg fault
        if ( index.column() < FNGLOBAL_ZOOM )
        {
            emit( dataChanged( index, index ) );
        }

        return true;
    }

    return false;
}

QVariant DataStore::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            return QString( Property::getNameAsString( (FN_PROPERTY)section ) );
        }
        else
        {
            return VPtr<Dataset>::asPtr( m_datasetList.at( section ) )->properties()->get( FNPROP_NAME ).toString();
        }
    }
    if ( role == Qt::UserRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            switch ( section )
            {
                case 1:
                    return QString( "GLuint" );
                    break;
                default:
                    break;
            }
        }
    }
    return QVariant();
}

QModelIndex DataStore::index( int row, int column, const QModelIndex & parent ) const
{
    if ( row < m_datasetList.size() )
    {
        return createIndex( row, column, VPtr<Dataset>::asPtr( m_datasetList.at( row ) ) );
    }
    else if ( m_datasetList.size() == 0 && column > FNGLOBAL_SAGITTAL )
    {
        return createIndex( row, column, 0 );
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex DataStore::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}

void DataStore::moveItemUp( int row )
{
    if ( row > 0 && row < m_datasetList.size() )
    {
        beginMoveRows( index( row, 0 ), row, row, index( row - 1, 0 ), row - 1 );
        m_datasetList.swap( row, row - 1 );
        endMoveRows();
        updateGlobals();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    }
}

void DataStore::moveItemDown( int row )
{
    if ( row >= 0 && row < m_datasetList.size() - 1 )
    {
        beginMoveRows( index( row, 0 ), row, row, index( row + 1, 0 ), row + 1 );
        m_datasetList.swap( row, row + 1 );
        endMoveRows();
        updateGlobals();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    }
}

void DataStore::deleteItem( int row )
{
    if ( row >= 0 && row < m_datasetList.size() )
    {
        beginRemoveRows( index( row, 0 ), row, row );
        Dataset* toDelete = VPtr<Dataset>::asPtr( m_datasetList.at( row ) );
        m_datasetList.removeAt( row );
        endRemoveRows();
        beginResetModel();
        reset();
        endResetModel();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
        delete toDelete;
        updateGlobals();
    }
}

void DataStore::propChanged()
{
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
}
