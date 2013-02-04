/*
 * selectionbox.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */
#include "selectionbox.h"

#include "../gui/gl/boxrenderer.h"

#include <QtCore/QAbstractItemModel>

SelectionBox::SelectionBox( QString name, QAbstractItemModel* globals ) :
    ROI( name, globals ),
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


    m_properties.set( Fn::ROI::X, x, 0., xMax, true );
    m_properties.set( Fn::ROI::Y, y, 0., yMax, true );
    m_properties.set( Fn::ROI::Z, z, 0., zMax, true );
    m_properties.set( Fn::ROI::DX, 5., 0., xMax/2, true );
    m_properties.set( Fn::ROI::DY, 5., 0., yMax/2, true );
    m_properties.set( Fn::ROI::DZ, 5., 0., zMax/2, true );
    m_properties.set( Fn::ROI::NEG, false, true );
    m_properties.set( Fn::ROI::TYPE, (int)Fn::ROIType::Box );
    m_properties.set( Fn::ROI::COLOR, QColor( 255, 0, 0 ), true );

    connect( &m_properties, SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
}

SelectionBox::~SelectionBox()
{
}

void SelectionBox::propChanged()
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

void SelectionBox::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse )
{
    if ( !m_renderer )
    {
        m_renderer = new BoxRenderer();
        m_renderer->init();
        m_renderer->updateGeometry( m_properties.get( Fn::ROI::X ).toFloat(),
                                    m_properties.get( Fn::ROI::Y ).toFloat(),
                                    m_properties.get( Fn::ROI::Z ).toFloat(),
                                    m_properties.get( Fn::ROI::DX ).toFloat(),
                                    m_properties.get( Fn::ROI::DY ).toFloat(),
                                    m_properties.get( Fn::ROI::DZ ).toFloat(),
                                    m_properties.get( Fn::ROI::COLOR ).value<QColor>() );
    }
    m_renderer->draw( mvpMatrix );
}

