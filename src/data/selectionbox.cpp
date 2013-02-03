/*
 * selectionbox.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#include "selectionbox.h"

SelectionBox::SelectionBox( QString name, int x, int y, int z, float dx, float dy, float dz, bool neg ) :
    ROI( name )
{
    m_properties.set( Fn::ROI::X, x, true );
    m_properties.set( Fn::ROI::Y, y, true );
    m_properties.set( Fn::ROI::Z, z, true );
    m_properties.set( Fn::ROI::DX, dx, true );
    m_properties.set( Fn::ROI::DY, dy, true );
    m_properties.set( Fn::ROI::DZ, dz, true );
    m_properties.set( Fn::ROI::NEG, neg, true );
    m_properties.set( Fn::ROI::TYPE, (int)Fn::ROIType::Box );
}

SelectionBox::~SelectionBox()
{
}
