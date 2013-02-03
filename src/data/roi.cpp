/*
 * roi.cpp
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#include "roi.h"

ROI::ROI( QString name )
{
    m_properties.set( Fn::Property::NAME, name );
    m_properties.set( Fn::Property::ACTIVE, true );
}

ROI::~ROI()
{
}

PropertyGroup* ROI::properties()
{
    return &m_properties;
}
