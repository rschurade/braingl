/*
 * globalpropertymodel.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */
#include "globalpropertymodel.h"

#include "enums.h"

#include <QtCore/QPoint>

GlobalPropertyModel::GlobalPropertyModel()
{
    m_properties.set( Fn::Property::NAME, QString("global properties") );
    m_properties.set( Fn::Property::GLOBAL_AXIAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_CORONAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_SAGITTAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_MAX_AXIAL, 1 );
    m_properties.set( Fn::Property::GLOBAL_MAX_CORONAL, 1 );
    m_properties.set( Fn::Property::GLOBAL_MAX_SAGITTAL, 1 );
    m_properties.set( Fn::Property::GLOBAL_SLICE_DX, 1.0f );
    m_properties.set( Fn::Property::GLOBAL_SLICE_DY, 1.0f );
    m_properties.set( Fn::Property::GLOBAL_SLICE_DZ, 1.0f );
    m_properties.set( Fn::Property::GLOBAL_LAST_PATH, QString("") );
    m_properties.set( Fn::Property::GLOBAL_SHOW_AXIAL, true );
    m_properties.set( Fn::Property::GLOBAL_SHOW_CORONAL, true );
    m_properties.set( Fn::Property::GLOBAL_SHOW_SAGITTAL, true );
    m_properties.set( Fn::Property::GLOBAL_CORONAL_AXIAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_SAGITTAL_AXIAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_SAGITTAL_CORONAL, 0 );
    m_properties.set( Fn::Property::GLOBAL_ZOOM, 1.0f );
    m_properties.set( Fn::Property::GLOBAL_MOVEX, 0 );
    m_properties.set( Fn::Property::GLOBAL_MOVEY, 0 );
    m_properties.set( Fn::Property::GLOBAL_BBX, 0 );
    m_properties.set( Fn::Property::GLOBAL_BBY, 0 );
    m_properties.set( Fn::Property::GLOBAL_VIEW, 0 );
    m_properties.set( Fn::Property::SETTING_RENDER_CROSSHAIRS, true );
}

GlobalPropertyModel::~GlobalPropertyModel()
{
}

int GlobalPropertyModel::rowCount( const QModelIndex &parent ) const
{
    return m_properties.size();
}

int GlobalPropertyModel::columnCount( const QModelIndex &parent ) const
{
    return 1;
}

QVariant GlobalPropertyModel::data( const QModelIndex &index, int role ) const
{
    return m_properties.get( (Fn::Property)index.row() );
}

QVariant GlobalPropertyModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Vertical )
        {
            return QString( Property::getNameAsString( (Fn::Property)section ) );
        }

    }
    return QVariant();
}

QModelIndex GlobalPropertyModel::index( int row, int column, const QModelIndex & parent ) const
{
    if ( m_properties.contains( row ) )
    {
        return createIndex( row, 0, 0 );
    }
    return QModelIndex();
}

QModelIndex GlobalPropertyModel::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}

bool GlobalPropertyModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    switch( (Fn::Property)index.row() )
    {
        case Fn::Property::GLOBAL_CORONAL_AXIAL:
            m_properties.set( Fn::Property::GLOBAL_CORONAL, value.toPoint().x() );
            m_properties.set( Fn::Property::GLOBAL_AXIAL, value.toPoint().y() );
            break;
        case Fn::Property::GLOBAL_SAGITTAL_AXIAL:
            m_properties.set( Fn::Property::GLOBAL_SAGITTAL, value.toPoint().x() );
            m_properties.set( Fn::Property::GLOBAL_AXIAL, value.toPoint().y() );
            break;
        case Fn::Property::GLOBAL_SAGITTAL_CORONAL:
            m_properties.set( Fn::Property::GLOBAL_SAGITTAL, value.toPoint().x() );
            m_properties.set( Fn::Property::GLOBAL_CORONAL, value.toPoint().y() );
            break;
        default:
            m_properties.set( (Fn::Property)index.row(), value );
            break;
    }
    // zoom - bby are updated in the render loop, emiting their changes causes an infinite event loop and seg fault
    if ( index.row() < (int)Fn::Property::GLOBAL_ZOOM )
    {
        emit dataChanged( index, index );
    }

    return true;
}
