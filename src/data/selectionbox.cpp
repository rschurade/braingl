/*
 * selectionbox.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */
#include "selectionbox.h"

#include <QtCore/QAbstractItemModel>

SelectionBox::SelectionBox( QString name, QAbstractItemModel* globals ) :
    ROI( name, globals )
{
    int x = globals->data( globals->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    int y = globals->data( globals->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    int z = globals->data( globals->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();

    int xMax = globals->data( globals->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toInt();
    int yMax = globals->data( globals->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toInt();
    int zMax = globals->data( globals->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toInt();


    m_properties.set( Fn::ROI::X, x, 0, xMax, true );
    m_properties.set( Fn::ROI::Y, y, 0, yMax, true );
    m_properties.set( Fn::ROI::Z, z, 0, zMax, true );
    m_properties.set( Fn::ROI::DX, 5, 0, xMax/2, true );
    m_properties.set( Fn::ROI::DY, 5, 0, yMax/2, true );
    m_properties.set( Fn::ROI::DZ, 5, 0, zMax/2, true );
    m_properties.set( Fn::ROI::NEG, false, true );
    m_properties.set( Fn::ROI::TYPE, (int)Fn::ROIType::Box );
}

SelectionBox::~SelectionBox()
{
}
