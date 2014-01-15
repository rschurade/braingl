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
    using namespace Fn;

    m_properties = new PropertyGroup();
    m_properties->createList( Fn::Property::G_CAMERA_TYPE, { "arcball", "camera" }, 0, "general" );
    m_properties->createList( Fn::Property::G_CAMERA_PROJECTION, { "orthogonal", "perspective" }, 0, "general" );

    m_properties->createFloat( Fn::Property::G_SAGITTAL, 0.0f, 0.0f, 0.0f, "general" );
    m_properties->createFloat( Fn::Property::G_CORONAL, 0.0f, 0.0f, 0.0f, "general" );
    m_properties->createFloat( Fn::Property::G_AXIAL, 0.0f, 0.0f, 0.0f, "general" );
    m_properties->createInt( Fn::Property::G_MAX_AXIAL, 1 );
    m_properties->createInt( Fn::Property::G_MAX_CORONAL, 1 );
    m_properties->createInt( Fn::Property::G_MAX_SAGITTAL, 1 );
    m_properties->createFloat( Fn::Property::G_SLICE_DX, 1.0f );
    m_properties->createFloat( Fn::Property::G_SLICE_DY, 1.0f );
    m_properties->createFloat( Fn::Property::G_SLICE_DZ, 1.0f );
    m_properties->createString( Fn::Property::G_LAST_PATH, QString("") );
    m_properties->createBool( Fn::Property::G_SHOW_AXIAL, true );
    m_properties->createBool( Fn::Property::G_SHOW_CORONAL, true );
    m_properties->createBool( Fn::Property::G_SHOW_SAGITTAL, true );
    m_properties->createInt( Fn::Property::G_CORONAL_AXIAL, 0 );
    m_properties->createInt( Fn::Property::G_SAGITTAL_AXIAL, 0 );
    m_properties->createInt( Fn::Property::G_SAGITTAL_CORONAL, 0 );
    m_properties->createBool( Fn::Property::G_NEED_SHADER_UPDATE, false );
    m_properties->createFloat( Fn::Property::G_ZOOM, 1.0f );
    m_properties->createInt( Fn::Property::G_MOVEX, 0 );
    m_properties->createInt( Fn::Property::G_MOVEY, 0 );

    m_properties->createFloat( Fn::Property::G_CAMERA_POS_X, 0.0f, -1000.0f, 1000.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_POS_Y, 0.0f, -1000.0f, 1000.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_POS_Z, 40.0f, -1000.0f, 1000.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_LOOKAT_X, 80.0f, -1000.f, 1000.f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_LOOKAT_Y, 100.0f, -1000.f, 1000.f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_LOOKAT_Z, 80.0f, -1000.f, 1000.f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_UP_X, 0.0f, -2.0f, 2.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_UP_Y, 0.0f, -2.0f, 2.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_UP_Z, 1.0f, -2.0f, 2.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_NEAR, 2.0f, -500.0f, 100.0f, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_FAR, -10.0f, -100.0f, 100.0f, "camera" );
    m_properties->createInt( Fn::Property::G_CAMERA_ANGLE, 45, 1, 180, "camera" );
    m_properties->createFloat( Fn::Property::G_CAMERA_KEYBOARD_STEP, 2.0f, 1.0f, 20.f, "camera" );
    m_properties->createBool( Fn::Property::G_CAMERA_FIXATE_Z, false, "camera" );
    m_properties->createInt( Fn::Property::G_CAMERA_FULLCIRCLE_STEPS, 360, 1, 360, "camera" );

    m_properties->createFloat( Fn::Property::G_ARCBALL_DISTANCE, 500.0f, 1.0f, 20000.0f, "arcball" );

    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_MAIN, QColor( 255, 255, 255 ), "general" );
    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_MAIN2, QColor( 255, 255, 255 ), "general" );
    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_COMBINED, QColor( 255, 255, 255 ), "general" );
    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_NAV1, QColor( 255, 255, 255 ), "general" );
    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_NAV2, QColor( 255, 255, 255 ), "general" );
    m_properties->createColor( Fn::Property::G_BACKGROUND_COLOR_NAV3, QColor( 255, 255, 255 ), "general" );
    m_properties->createBool( Fn::Property::G_RENDER_CROSSHAIRS, true, "general" );
    m_properties->createBool( Fn::Property::G_RENDER_CROSSHAIRS_2, true, "general" );
    m_properties->createColor( Fn::Property::G_CROSSHAIR_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties->createBool( Fn::Property::G_SHOW_NAV_SLIDERS, true, "general" );
    m_properties->createList( Fn::Property::G_TRANSPARENCY, {"classic", "new"}, 0, "general" );
    m_properties->createBool( Fn::Property::G_UNSELECTED_FIBERS_GREY, false, "general" );

    m_properties->createInt( Fn::Property::G_SCREENSHOT_QUALITY, 1, 50, 1000000 );
    m_properties->createInt( Fn::Property::G_SCREENSHOT_QUALITY, 1, 50, 1000000 );
    m_properties->createDir( Fn::Property::G_SCREENSHOT_PATH, QDir(""), "screenshot" );
    m_properties->createString( Fn::Property::G_SCREENSHOT_PREFIX, QString( "screenshot_" ) , "screenshot" );
    m_properties->createString( Fn::Property::G_SCREENSHOT_PREFIX2, QString( "screenshot_gl2_" ) , "screenshot" );
    m_properties->createInt( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER, 0, 0, std::numeric_limits<int>().max(), "screenshot" );
    m_properties->createInt( Fn::Property::G_SCREENSHOT_DIGITS, 4, 1, 10 , "screenshot" );
    m_properties->createInt( Fn::Property::G_SCREENSHOT_WIDTH, 1920, 50, 4096, "screenshot" );
    m_properties->createInt( Fn::Property::G_SCREENSHOT_HEIGHT, 1200, 50, 4096, "screenshot" );
    m_properties->createBool( Fn::Property::G_SCREENSHOT_KEEP_ASPECT, false, "screenshot" );
    m_properties->createFloat( Fn::Property::G_SCREENSHOT_SIZE_RATIO, 1.0f );
    m_properties->createList( Fn::Property::G_SCREENSHOT_SIZE_SELECTION, { Resolution2String::s( Resolution::R_1024x768 ),
                                                                       Resolution2String::s( Resolution::R_1920x1080 ),
                                                                       Resolution2String::s( Resolution::R_1920x1200 ),
    }, 0, "screenshot" );
    m_properties->createButton( Fn::Property::G_SCREENSHOT_COPY_CURRENT, "screenshot" );
    m_properties->createInt( Fn::Property::G_WIDTH_MAINGL, 1 );
    m_properties->createInt( Fn::Property::G_HEIGHT_MAINGL, 1 );
    m_properties->createInt( Fn::Property::G_WIDTH_MAINGL2, 1 );
    m_properties->createInt( Fn::Property::G_HEIGHT_MAINGL2, 1 );

    m_properties->createBool( Fn::Property::G_SCREENSHOT_DO_MAINGL, true, "screenshot" );
    m_properties->createBool( Fn::Property::G_SCREENSHOT_DO_MAINGL2, false, "screenshot" );
    m_properties->createBool( Fn::Property::G_SCREENSHOT_STEREOSCOPIC, false, "screenshot" );

    m_properties->createBool( Fn::Property::G_LIGHT_SWITCH, true, "light" );
    m_properties->createFloat( Fn::Property::G_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties->createFloat( Fn::Property::G_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );

    m_properties->createInt( Fn::Property::G_FILTER_SIZE, 1, 1, 20, "algos" );
    m_properties->createInt( Fn::Property::G_MIN_COMPONENT_SIZE, 1000, 1, 100000, "algos" );
    m_properties->createFloat( Fn::Property::G_DECIMATE_EPSILON, 2.0f, 0.0f, 20.f, "algos" );

    connect( m_properties->getProperty( Fn::Property::G_SCREENSHOT_WIDTH ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotScreenShotWidth( QVariant ) ) );
    connect( m_properties->getProperty( Fn::Property::G_SCREENSHOT_HEIGHT ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotScreenShotHeight( QVariant ) ) );
    connect( m_properties->getProperty( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotScreenShotKeepAspect( QVariant ) ) );
    connect( m_properties->getProperty( Fn::Property::G_SCREENSHOT_SIZE_SELECTION ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotScreenShotPredefined( QVariant ) ) );
    connect( m_properties->getProperty( Fn::Property::G_SCREENSHOT_COPY_CURRENT ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotScreenShotCopyCurrent( QVariant ) ) );


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
        case Fn::Property::G_SCREENSHOT_QUALITY:
        {
            m_properties->set( (Fn::Property)index.row(), value );
            m_properties->setMax( Fn::Property::G_SCREENSHOT_WIDTH, value );
            m_properties->setMax( Fn::Property::G_SCREENSHOT_HEIGHT, value );
            break;
        }
        default:
            m_properties->set( (Fn::Property)index.row(), value );
            break;
    }

    return true;
}

bool GlobalPropertyModel::submit()
{
    m_properties->setMax( Fn::Property::G_AXIAL, m_properties->get( Fn::Property::G_MAX_AXIAL ).toInt() - 1 );
    m_properties->setMax( Fn::Property::G_CORONAL, m_properties->get( Fn::Property::G_MAX_CORONAL ).toInt() - 1 );
    m_properties->setMax( Fn::Property::G_SAGITTAL, m_properties->get( Fn::Property::G_MAX_SAGITTAL ).toInt() - 1 );
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    return true;
}

void GlobalPropertyModel::slotScreenShotWidth( QVariant value )
{
    float maxSize = m_properties->get( Fn::Property::G_SCREENSHOT_QUALITY ).toFloat();
    float ratio = m_properties->get( Fn::Property::G_SCREENSHOT_SIZE_RATIO ).toFloat();

    if ( m_properties->get( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ).toBool() )
    {
        m_properties->setMax( Fn::Property::G_SCREENSHOT_HEIGHT, qMin( maxSize, maxSize / ratio ) );
        m_properties->set( Fn::Property::G_SCREENSHOT_HEIGHT, value.toFloat() / ratio );
    }

}

void GlobalPropertyModel::slotScreenShotHeight( QVariant value )
{
    float maxSize = m_properties->get( Fn::Property::G_SCREENSHOT_QUALITY ).toFloat();
    float ratio = m_properties->get( Fn::Property::G_SCREENSHOT_SIZE_RATIO ).toFloat();

    if ( m_properties->get( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ).toBool() )
    {
        m_properties->setMax( Fn::Property::G_SCREENSHOT_WIDTH, qMin( maxSize, maxSize * ratio ) );
        m_properties->set( Fn::Property::G_SCREENSHOT_WIDTH, value.toFloat() * ratio );
    }
}

void GlobalPropertyModel::slotScreenShotKeepAspect( QVariant value )
{
    int maxSize = m_properties->get( Fn::Property::G_SCREENSHOT_QUALITY ).toInt();
    if ( value.toBool() )
    {
        float width = m_properties->get( Fn::Property::G_SCREENSHOT_WIDTH ).toFloat();
        float height = m_properties->get( Fn::Property::G_SCREENSHOT_HEIGHT ).toFloat();
        m_properties->set( Fn::Property::G_SCREENSHOT_SIZE_RATIO, width / height );
    }
    else
    {
        m_properties->setMax( Fn::Property::G_SCREENSHOT_WIDTH, maxSize );
        m_properties->setMax( Fn::Property::G_SCREENSHOT_HEIGHT, maxSize );
    }
}

void GlobalPropertyModel::slotScreenShotPredefined( QVariant value )
{
    m_properties->set( Fn::Property::G_SCREENSHOT_KEEP_ASPECT, false );
    switch( (Fn::Resolution)value.toInt() )
    {
        case Fn::Resolution::R_1024x768:
            m_properties->set( Fn::Property::G_SCREENSHOT_WIDTH, 1024 );
            m_properties->set( Fn::Property::G_SCREENSHOT_HEIGHT, 768 );
            break;
        case Fn::Resolution::R_1920x1080:
            m_properties->set( Fn::Property::G_SCREENSHOT_WIDTH, 1920 );
            m_properties->set( Fn::Property::G_SCREENSHOT_HEIGHT, 1080 );
            break;
        case Fn::Resolution::R_1920x1200:
            m_properties->set( Fn::Property::G_SCREENSHOT_WIDTH, 1920 );
            m_properties->set( Fn::Property::G_SCREENSHOT_HEIGHT, 1200 );
            break;
    }
}

void GlobalPropertyModel::slotScreenShotCopyCurrent( QVariant value )
{
    m_properties->set( Fn::Property::G_SCREENSHOT_WIDTH, m_properties->get( Fn::Property::G_WIDTH_MAINGL ) );
    m_properties->set( Fn::Property::G_SCREENSHOT_HEIGHT, m_properties->get( Fn::Property::G_HEIGHT_MAINGL ) );

}

QList<QVariant> GlobalPropertyModel::getState()
{
    return m_properties->getState();
}

void GlobalPropertyModel::setState( QList<QVariant> state )
{
    m_properties->setState( state );
}
