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

ROIBox::ROIBox() :
    ROI( QString("new roi") + QString::number( ROI::m_count++ ) )
{
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    float x = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * dx;
    float y = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * dy;
    float z = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * dz;

    float xMax = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat() * dx;
    float yMax = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat() * dy;
    float zMax = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat() * dz;

    m_properties.set( Fn::ROI::RENDER, true, true );
    m_properties.set( Fn::ROI::NEG, false, true );
    m_properties.set( Fn::ROI::SHAPE, { "ellipsoid", "sphere", "cube", "box" }, 0, true );
    m_properties.set( Fn::ROI::X, x, 0., xMax, true );
    m_properties.set( Fn::ROI::Y, y, 0., yMax, true );
    m_properties.set( Fn::ROI::Z, z, 0., zMax, true );
    m_properties.set( Fn::ROI::DX, 20., 0., xMax, true );
    m_properties.set( Fn::ROI::DY, 20., 0., yMax, true );
    m_properties.set( Fn::ROI::DZ, 20., 0., zMax, true );
    m_properties.set( Fn::ROI::TYPE, (int)Fn::ROIType::Box );
    m_properties.set( Fn::ROI::COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::ROI::ALPHA, 0.4f, 0.f, 1.f, true );
    m_properties.set( Fn::ROI::ID, m_count );
    m_properties.set( Fn::ROI::PICK_ID, 0 );
    m_properties.set( Fn::ROI::STICK_TO_CROSSHAIR, false, true );

    m_properties.set( Fn::ROI::PICK_ID, (int)GLFunctions::getPickIndex() );

    connect( &m_properties, SIGNAL( signalPropChanged( int ) ), this, SLOT( propChanged() ) );
    connect( m_properties.getProperty( Fn::ROI::DX ), SIGNAL( valueChanged( float ) ), this, SLOT( dxChanged( float ) ) );
    connect( Models::g(), SIGNAL(  dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );
}

ROIBox::~ROIBox()
{
}

void ROIBox::globalChanged()
{
    if (  m_properties.get( Fn::ROI::STICK_TO_CROSSHAIR ).toBool() )
    {
        float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
        float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
        float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

        float x = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * dx;
        float y = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * dy;
        float z = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * dz;

        m_properties.set( Fn::ROI::X, x );
        m_properties.set( Fn::ROI::Y, y );
        m_properties.set( Fn::ROI::Z, z );
        m_properties.slotPropChanged();
    }
}

void ROIBox::propChanged()
{
    if (  m_properties.get( Fn::ROI::STICK_TO_CROSSHAIR ).toBool() )
    {
        float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
        float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
        float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

        float x = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * dx;
        float y = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * dy;
        float z = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * dz;

        m_properties.set( Fn::ROI::X, x );
        m_properties.set( Fn::ROI::Y, y );
        m_properties.set( Fn::ROI::Z, z );
    }
    int shape = m_properties.get( Fn::ROI::SHAPE ).toInt();

    if ( shape == 1 || shape == 2 )
    {
        m_properties.getWidget( Fn::ROI::DY )->hide();
        m_properties.getWidget( Fn::ROI::DZ )->hide();
    }

    if ( shape == 0 || shape == 3 )
    {
        m_properties.getWidget( Fn::ROI::DY )->show();
        m_properties.getWidget( Fn::ROI::DZ )->show();
    }
}

void ROIBox::dxChanged( float value )
{
    int shape = m_properties.get( Fn::ROI::SHAPE ).toInt();
    if ( shape == 1 || shape == 2 )
    {
        m_properties.set( Fn::ROI::DY, value );
        m_properties.set( Fn::ROI::DZ, value );
    }
}

void ROIBox::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
        float x = m_properties.get( Fn::ROI::X ).toFloat();
        float y = m_properties.get( Fn::ROI::Y ).toFloat();
        float z = m_properties.get( Fn::ROI::Z ).toFloat();
        float dx = m_properties.get( Fn::ROI::DX ).toFloat();
        float dy = m_properties.get( Fn::ROI::DY ).toFloat();
        float dz = m_properties.get( Fn::ROI::DZ ).toFloat();
        QColor color = m_properties.get( Fn::ROI::COLOR ).value<QColor>();
        color.setAlphaF( m_properties.get( Fn::ROI::ALPHA ).toFloat() );
        int pickID = m_properties.get( Fn::ROI::PICK_ID ).toInt();

    if ( m_properties.get( Fn::ROI::RENDER).toBool() )
    {
        if ( m_properties.get( Fn::ROI::SHAPE ).toInt() == 0 || m_properties.get( Fn::ROI::SHAPE ).toInt() == 1 )
        {
            GLFunctions::drawSphere( pMatrix, mvMatrix, x, y ,z, dx, dy, dz, color, pickID, width, height, renderMode );
        }
        else
        {
            GLFunctions::drawBox( pMatrix, mvMatrix, x, y ,z, dx, dy, dz, color, pickID, width, height, renderMode );
        }
    }
}

