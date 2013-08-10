/*
 * globalpropertymodel.cpp
 *
 * Created on: 02.02.2013
 * @author Ralph Schurade
 */
#include "globalpropertymodel.h"

#include "enums.h"
#include "vptr.h"

#include <QDebug>
#include <QPoint>
#include <QColor>

GlobalPropertyModel::GlobalPropertyModel()
{
    m_properties = new PropertyGroup();
    m_properties->create( Fn::Property::G_SAGITTAL, 0, 0, 0, "general" );
    m_properties->create( Fn::Property::G_CORONAL, 0, 0, 0, "general" );
    m_properties->create( Fn::Property::G_AXIAL, 0, 0, 0, "general" );
    m_properties->create( Fn::Property::G_MAX_AXIAL, 1 );
    m_properties->create( Fn::Property::G_MAX_CORONAL, 1 );
    m_properties->create( Fn::Property::G_MAX_SAGITTAL, 1 );
    m_properties->create( Fn::Property::G_SLICE_DX, 1.0f );
    m_properties->create( Fn::Property::G_SLICE_DY, 1.0f );
    m_properties->create( Fn::Property::G_SLICE_DZ, 1.0f );
    m_properties->create( Fn::Property::G_LAST_PATH, QString("") );
    m_properties->create( Fn::Property::G_SHOW_AXIAL, true );
    m_properties->create( Fn::Property::G_SHOW_CORONAL, true );
    m_properties->create( Fn::Property::G_SHOW_SAGITTAL, true );
    m_properties->create( Fn::Property::G_CORONAL_AXIAL, 0 );
    m_properties->create( Fn::Property::G_SAGITTAL_AXIAL, 0 );
    m_properties->create( Fn::Property::G_SAGITTAL_CORONAL, 0 );
    m_properties->create( Fn::Property::G_NEED_SHADER_UPDATE, false );
    m_properties->create( Fn::Property::G_ZOOM, 1.0f );
    m_properties->create( Fn::Property::G_MOVEX, 0 );
    m_properties->create( Fn::Property::G_MOVEY, 0 );
    m_properties->create( Fn::Property::G_CAMERA_TYPE, { "arcball", "camera" }, 0, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_PROJECTION, { "orthogonal", "perspective" }, 0, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_POS_X, 0, -1000, 1000, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_POS_Y, 0, -1000, 1000, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_POS_Z, 40, -1000, 1000, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_LOOKAT_X, 80.0f, -1000.f, 1000.f, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_LOOKAT_Y, 100.0f, -1000.f, 1000.f, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_LOOKAT_Z, 80.0f, -1000.f, 1000.f, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_UP_X, 0, -1, 1, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_UP_Y, 0, -1, 1, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_UP_Z, 1, -1, 1, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_NEAR, 2.0f, -500.0f, 100.0f, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_FAR, -10.0f, -100.0f, 100.0f, "camera" );
    m_properties->create( Fn::Property::G_CAMERA_ANGLE, 90, 1, 180, "camera" );

    m_properties->create( Fn::Property::G_BACKGROUND_COLOR_MAIN, QColor( 255, 255, 255 ), "general" );
    m_properties->create( Fn::Property::G_BACKGROUND_COLOR_COMBINED, QColor( 255, 255, 255 ), "general" );
    m_properties->create( Fn::Property::G_BACKGROUND_COLOR_NAV1, QColor( 255, 255, 255 ), "general" );
    m_properties->create( Fn::Property::G_BACKGROUND_COLOR_NAV2, QColor( 255, 255, 255 ), "general" );
    m_properties->create( Fn::Property::G_BACKGROUND_COLOR_NAV3, QColor( 255, 255, 255 ), "general" );
    m_properties->create( Fn::Property::G_RENDER_CROSSHAIRS, true, "general" );
    m_properties->create( Fn::Property::G_CROSSHAIR_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties->create( Fn::Property::G_SHOW_NAV_SLIDERS, true, "general" );
    m_properties->create( Fn::Property::G_SCREENSHOT_QUALITY, 1, 1, 5, "general" );
    m_properties->create( Fn::Property::G_SCREENSHOT_PATH, QDir(""), "general" );
    m_properties->create( Fn::Property::G_TRANSPARENCY, {"classic", "new"}, 0, "general" );
    connect( m_properties, SIGNAL( signalPropChanged() ), this, SLOT( submit() ) );
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
    if ( index.row() == (int)Fn::Property::G_OBJECT )
    {
        return VPtr<PropertyGroup>::asQVariant( m_properties );
    }
    else
    {
        return m_properties->get( (Fn::Property)index.row() );
    }
}

QVariant GlobalPropertyModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Vertical )
        {
            return QString( Fn::Prop2String::s( (Fn::Property)section ) );
        }

    }
    return QVariant();
}

QModelIndex GlobalPropertyModel::index( int row, int column, const QModelIndex & parent ) const
{
    if ( m_properties->contains( (Fn::Property)row ) )
    {
        return createIndex( row, 0, 0 );
    }
    else if ( row == (int)Fn::Property::G_OBJECT )
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
        case Fn::Property::G_CORONAL_AXIAL:
            m_properties->set( Fn::Property::G_CORONAL, value.toPoint().x() );
            m_properties->set( Fn::Property::G_AXIAL, value.toPoint().y() );
            break;
        case Fn::Property::G_SAGITTAL_AXIAL:
            m_properties->set( Fn::Property::G_SAGITTAL, value.toPoint().x() );
            m_properties->set( Fn::Property::G_AXIAL, value.toPoint().y() );
            break;
        case Fn::Property::G_SAGITTAL_CORONAL:
            m_properties->set( Fn::Property::G_SAGITTAL, value.toPoint().x() );
            m_properties->set( Fn::Property::G_CORONAL, value.toPoint().y() );
            break;
        default:
            m_properties->set( (Fn::Property)index.row(), value );
            break;
    }

    return true;
}

bool GlobalPropertyModel::submit()
{
    m_properties->setMaxI( Fn::Property::G_AXIAL, m_properties->get( Fn::Property::G_MAX_AXIAL ).toInt() - 1 );
    m_properties->setMaxI( Fn::Property::G_CORONAL, m_properties->get( Fn::Property::G_MAX_CORONAL ).toInt() - 1 );
    m_properties->setMaxI( Fn::Property::G_SAGITTAL, m_properties->get( Fn::Property::G_MAX_SAGITTAL ).toInt() - 1 );
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    return true;
}
