/*
 * roibox.cpp
 *
 * Created on: 02.02.2013
 * @author Ralph Schurade
 */
#include "roibox.h"

#include "models.h"

#include "../gui/gl/glfunctions.h"

#include <QAbstractItemModel>
#include <QWidget>

ROIBox::ROIBox() :
    ROI( QString("new roi") + QString::number( ROI::m_count++ ) )
{
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

    float x = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toFloat() * dx;
    float y = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toFloat() * dy;
    float z = Models::g()->data( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toFloat() * dz;

    float xMax = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_SAGITTAL, 0 ) ).toFloat() * dx;
    float yMax = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_CORONAL, 0 ) ).toFloat() * dy;
    float zMax = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_AXIAL, 0 ) ).toFloat() * dz;

    m_properties.createBool( Fn::Property::D_RENDER, true, "general" );
    m_properties.createBool( Fn::Property::D_NEG, false, "general" );
    m_properties.createList( Fn::Property::D_SHAPE, { "ellipsoid", "sphere", "cube", "box" }, 0, "general" );
    m_properties.createFloat( Fn::Property::D_X, x, 0., xMax, "general" );
    m_properties.createFloat( Fn::Property::D_Y, y, 0., yMax, "general" );
    m_properties.createFloat( Fn::Property::D_Z, z, 0., zMax, "general" );
    m_properties.createFloat( Fn::Property::D_DX, 20., 0., xMax, "general" );
    m_properties.createFloat( Fn::Property::D_DY, 20., 0., yMax, "general" );
    m_properties.createFloat( Fn::Property::D_DZ, 20., 0., zMax, "general" );
    m_properties.createInt( Fn::Property::D_TYPE, (int)Fn::ROIType::Box );
    m_properties.createColor( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties.createFloat( Fn::Property::D_ALPHA, 0.4f, 0.f, 1.f, "general" );
    m_properties.createBool( Fn::Property::D_STICK_TO_CROSSHAIR, false, "general" );
    m_properties.createInt( Fn::Property::D_PICK_ID, (int)GLFunctions::getPickIndex() );

    connect( &m_properties, SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
    connect( Models::g(), SIGNAL(  dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( propChanged() ) );

    //qDebug() << "new ROI created: " << m_properties.get( Fn::Property::D_ID ).toInt();
}

ROIBox::~ROIBox()
{
    //qDebug() << "ROI deleted: " << m_properties.get( Fn::Property::D_ID ).toInt();
}

void ROIBox::propChanged()
{
    if (  m_properties.get( Fn::Property::D_STICK_TO_CROSSHAIR ).toBool() )
    {
        float dx = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
        float dy = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
        float dz = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

        float x = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toFloat() * dx;
        float y = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toFloat() * dy;
        float z = Models::g()->data( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toFloat() * dz;

        m_properties.set( Fn::Property::D_X, x );
        m_properties.set( Fn::Property::D_Y, y );
        m_properties.set( Fn::Property::D_Z, z );
    }
    int shape = m_properties.get( Fn::Property::D_SHAPE ).toInt();

    if ( shape == 1 || shape == 2 )
    {
        float dx = m_properties.get( Fn::Property::D_DX ).toFloat();
        m_properties.set( Fn::Property::D_DY, dx );
        m_properties.set( Fn::Property::D_DZ, dx );

        m_properties.getWidget( Fn::Property::D_DY )->hide();
        m_properties.getWidget( Fn::Property::D_DZ )->hide();
    }

    if ( shape == 0 || shape == 3 )
    {
        m_properties.getWidget( Fn::Property::D_DY )->show();
        m_properties.getWidget( Fn::Property::D_DZ )->show();
    }
    //qDebug() << "emitting: " << m_properties.get( Fn::Property::D_ID ).toInt();
    emit ( signalPropChanged( m_properties.get( Fn::Property::D_ID ).toInt() ) );
}

void ROIBox::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
    float x = m_properties.get( Fn::Property::D_X ).toFloat();
    float y = m_properties.get( Fn::Property::D_Y ).toFloat();
    float z = m_properties.get( Fn::Property::D_Z ).toFloat();
    float dx = m_properties.get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties.get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties.get( Fn::Property::D_DZ ).toFloat();
    QColor color = m_properties.get( Fn::Property::D_COLOR ).value<QColor>();
    color.setAlphaF( m_properties.get( Fn::Property::D_ALPHA ).toFloat() );
    int pickID = m_properties.get( Fn::Property::D_PICK_ID ).toInt();

    if ( m_properties.get( Fn::Property::D_RENDER).toBool() )
    {
        if ( m_properties.get( Fn::Property::D_SHAPE ).toInt() == 0 || m_properties.get( Fn::Property::D_SHAPE ).toInt() == 1 )
        {
            GLFunctions::drawSphere( pMatrix, mvMatrix, x, y ,z, dx, dy, dz, color, pickID, width, height, renderMode );
        }
        else
        {
            GLFunctions::drawBox( pMatrix, mvMatrix, x, y ,z, dx, dy, dz, color, pickID, width, height, renderMode );
        }
    }
}

