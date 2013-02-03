/*
 * roi.cpp
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#include "roi.h"

ROI::ROI( QString name )
{
    m_properties.set( Fn::ROI::NAME, name );
    m_properties.set( Fn::ROI::ACTIVE, true );
}

ROI::~ROI()
{
}

ROIPropertyGroup* ROI::properties()
{
    return &m_properties;
}
