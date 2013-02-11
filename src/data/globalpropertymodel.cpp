/*
 * globalpropertymodel.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph Schurade
 */
#include "globalpropertymodel.h"

#include "enums.h"
#include "vptr.h"

#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtGui/QColor>

GlobalPropertyModel::GlobalPropertyModel()
{
    m_properties = new GlobalPropertyGroup();
    m_properties->set( Fn::Global::AXIAL, 0, 0, 0, true );
    m_properties->set( Fn::Global::CORONAL, 0, 0, 0, true );
    m_properties->set( Fn::Global::SAGITTAL, 0, 0, 0, true );
    m_properties->set( Fn::Global::MAX_AXIAL, 1 );
    m_properties->set( Fn::Global::MAX_CORONAL, 1 );
    m_properties->set( Fn::Global::MAX_SAGITTAL, 1 );
    m_properties->set( Fn::Global::SLICE_DX, 1.0f );
    m_properties->set( Fn::Global::SLICE_DY, 1.0f );
    m_properties->set( Fn::Global::SLICE_DZ, 1.0f );
    m_properties->set( Fn::Global::LAST_PATH, QString("") );
    m_properties->set( Fn::Global::SHOW_AXIAL, true );
    m_properties->set( Fn::Global::SHOW_CORONAL, true );
    m_properties->set( Fn::Global::SHOW_SAGITTAL, true );
    m_properties->set( Fn::Global::CORONAL_AXIAL, 0 );
    m_properties->set( Fn::Global::SAGITTAL_AXIAL, 0 );
    m_properties->set( Fn::Global::SAGITTAL_CORONAL, 0 );
    m_properties->set( Fn::Global::ZOOM, 1.0f );
    m_properties->set( Fn::Global::MOVEX, 0 );
    m_properties->set( Fn::Global::MOVEY, 0 );
    m_properties->set( Fn::Global::BBX, 0 );
    m_properties->set( Fn::Global::BBY, 0 );
    m_properties->set( Fn::Global::VIEW, 0 );
    m_properties->set( Fn::Global::BACKGROUND_COLOR_MAIN, QColor( 255, 255, 255 ), true );
    m_properties->set( Fn::Global::BACKGROUND_COLOR_COMBINED, QColor( 255, 255, 255 ), true );
    m_properties->set( Fn::Global::BACKGROUND_COLOR_NAV1, QColor( 255, 255, 255 ), true );
    m_properties->set( Fn::Global::BACKGROUND_COLOR_NAV2, QColor( 255, 255, 255 ), true );
    m_properties->set( Fn::Global::BACKGROUND_COLOR_NAV3, QColor( 255, 255, 255 ), true );
    m_properties->set( Fn::Global::RENDER_CROSSHAIRS, true, true );
    m_properties->set( Fn::Global::CROSSHAIR_COLOR, QColor( 255, 0, 0 ), true );
    m_properties->set( Fn::Global::SHOW_NAV_SLIDERS, true, true );
    m_properties->set( Fn::Global::SCREENSHOT_QUALITY, 1, 1, 5, true );
    connect( m_properties, SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
}

GlobalPropertyModel::~GlobalPropertyModel()
{
}

int GlobalPropertyModel::rowCount( const QModelIndex &parent ) const
{
    return m_properties->size();
}

int GlobalPropertyModel::columnCount( const QModelIndex &parent ) const
{
    return 1;
}

QVariant GlobalPropertyModel::data( const QModelIndex &index, int role ) const
{
    if ( index.row() == (int)Fn::Global::OBJECT )
    {
        return VPtr<GlobalPropertyGroup>::asQVariant( m_properties );
    }
    else
    {
        return m_properties->get( (Fn::Global)index.row() );
    }
}

QVariant GlobalPropertyModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Vertical )
        {
            return QString( Fn::Global2String::s( (Fn::Global)section ) );
        }

    }
    return QVariant();
}

QModelIndex GlobalPropertyModel::index( int row, int column, const QModelIndex & parent ) const
{
    if ( m_properties->contains( (Fn::Global)row ) )
    {
        return createIndex( row, 0, 0 );
    }
    else if ( row == (int)Fn::Global::OBJECT )
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
    switch( (Fn::Global)index.row() )
    {
        case Fn::Global::CORONAL_AXIAL:
            m_properties->set( Fn::Global::CORONAL, value.toPoint().x() );
            m_properties->set( Fn::Global::AXIAL, value.toPoint().y() );
            break;
        case Fn::Global::SAGITTAL_AXIAL:
            m_properties->set( Fn::Global::SAGITTAL, value.toPoint().x() );
            m_properties->set( Fn::Global::AXIAL, value.toPoint().y() );
            break;
        case Fn::Global::SAGITTAL_CORONAL:
            m_properties->set( Fn::Global::SAGITTAL, value.toPoint().x() );
            m_properties->set( Fn::Global::CORONAL, value.toPoint().y() );
            break;
        default:
            m_properties->set( (Fn::Global)index.row(), value );
            break;
    }
    // zoom - bby are updated in the render loop, emiting their changes causes an infinite event loop and seg fault
    if ( index.row() < (int)Fn::Global::ZOOM )
    {
        propChanged();
        emit dataChanged( index, index );
    }

    return true;
}

void GlobalPropertyModel::propChanged()
{
    m_properties->setMax( Fn::Global::AXIAL, m_properties->get( Fn::Global::MAX_AXIAL ).toInt() - 1 );
    m_properties->setMax( Fn::Global::CORONAL, m_properties->get( Fn::Global::MAX_CORONAL ).toInt() - 1 );
    m_properties->setMax( Fn::Global::SAGITTAL, m_properties->get( Fn::Global::MAX_SAGITTAL ).toInt() - 1 );
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
}
