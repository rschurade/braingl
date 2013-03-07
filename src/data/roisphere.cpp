/*
 * roisphere.cpp
 *
 *  Created on: 06.03.2013
 *      Author: Ralph Schurade
 */
#include "roisphere.h"

#include "../gui/gl/sphererenderer.h"

#include <QtCore/QAbstractItemModel>

int ROISphere::m_count = 0;

ROISphere::ROISphere( QAbstractItemModel* globals ) :
    ROI( QString("new sphere") + QString::number( m_count++ ), globals ),
    m_renderer( 0 )
{
    float dx = globals->data( globals->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = globals->data( globals->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = globals->data( globals->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    float x = globals->data( globals->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * dx;
    float y = globals->data( globals->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * dy;
    float z = globals->data( globals->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * dz;

    float xMax = globals->data( globals->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat() * dx;
    float yMax = globals->data( globals->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat() * dy;
    float zMax = globals->data( globals->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat() * dz;

    m_properties.set( Fn::ROI::RENDER, true, true );
    m_properties.set( Fn::ROI::X, x, 0., xMax, true );
    m_properties.set( Fn::ROI::Y, y, 0., yMax, true );
    m_properties.set( Fn::ROI::Z, z, 0., zMax, true );
    m_properties.set( Fn::ROI::DX, 20., 0., xMax/2, true );
    m_properties.set( Fn::ROI::DY, 20., 0., yMax/2, true );
    m_properties.set( Fn::ROI::DZ, 20., 0., zMax/2, true );
    m_properties.set( Fn::ROI::NEG, false, true );
    m_properties.set( Fn::ROI::TYPE, (int)Fn::ROIType::Sphere );
    m_properties.set( Fn::ROI::COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::ROI::ID, m_count );
    m_properties.set( Fn::ROI::PICK_ID, 0 );
    m_properties.set( Fn::ROI::STICK_TO_CROSSHAIR, false, true );

    connect( &m_properties, SIGNAL( signalPropChanged( int ) ), this, SLOT( propChanged() ) );
    connect( globals, SIGNAL(  dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );
}

ROISphere::~ROISphere()
{
}

void ROISphere::globalChanged()
{
    if (  m_properties.get( Fn::ROI::STICK_TO_CROSSHAIR ).toBool() )
    {
        float dx = m_globals->data( m_globals->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
        float dy = m_globals->data( m_globals->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
        float dz = m_globals->data( m_globals->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

        float x = m_globals->data( m_globals->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * dx;
        float y = m_globals->data( m_globals->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * dy;
        float z = m_globals->data( m_globals->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * dz;

        m_properties.set( Fn::ROI::X, x );
        m_properties.set( Fn::ROI::Y, y );
        m_properties.set( Fn::ROI::Z, z );
        m_properties.slotPropChanged();
    }
}

void ROISphere::propChanged()
{
    if ( m_renderer)
    {
        m_renderer->updateGeometry( m_properties.get( Fn::ROI::X ).toFloat(),
                                    m_properties.get( Fn::ROI::Y ).toFloat(),
                                    m_properties.get( Fn::ROI::Z ).toFloat(),
                                    m_properties.get( Fn::ROI::DX ).toFloat(),
                                    m_properties.get( Fn::ROI::DY ).toFloat(),
                                    m_properties.get( Fn::ROI::DZ ).toFloat(),
                                    m_properties.get( Fn::ROI::COLOR ).value<QColor>() );
    }
}

void ROISphere::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
    if ( !m_renderer )
    {
        m_renderer = new SphereRenderer();
        m_properties.set( Fn::ROI::PICK_ID, (int)m_renderer->getPickId() );
        m_renderer->init();
        m_renderer->updateGeometry( m_properties.get( Fn::ROI::X ).toFloat(),
                                    m_properties.get( Fn::ROI::Y ).toFloat(),
                                    m_properties.get( Fn::ROI::Z ).toFloat(),
                                    m_properties.get( Fn::ROI::DX ).toFloat(),
                                    m_properties.get( Fn::ROI::DY ).toFloat(),
                                    m_properties.get( Fn::ROI::DZ ).toFloat(),
                                    m_properties.get( Fn::ROI::COLOR ).value<QColor>() );
    }
    if ( m_properties.get( Fn::ROI::RENDER).toBool() )
    {
        m_renderer->draw( pMatrix, mvMatrix );
    }
}

