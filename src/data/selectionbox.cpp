/*
 * selectionbox.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#include "selectionbox.h"

SelectionBox::SelectionBox( QString name )
{
    m_properties.set( Fn::Property::NAME, name );
    m_properties.set( Fn::Property::ACTIVE, true );
}

SelectionBox::~SelectionBox()
{
}

PropertyGroup* SelectionBox::properties()
{
    return &m_properties;
}
