/*
 * roi.cpp
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#include "roi.h"

int ROI::m_count = 0;

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
