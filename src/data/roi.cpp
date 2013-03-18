/*
 * roi.cpp
 *
 *  Created on: 03.02.2013
 *      Author: Ralph Schurade
 */

#include "roi.h"

ROI::ROI( QString name )
{
    m_properties.set( Fn::ROI::NAME, name, true );
    m_properties.set( Fn::ROI::ACTIVE, true, true );
}

ROI::~ROI()
{
}

ROIPropertyGroup* ROI::properties()
{
    return &m_properties;
}
