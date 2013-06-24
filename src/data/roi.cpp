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
    m_properties.set( Fn::Property::D_NAME, name, "general" );
    m_properties.set( Fn::Property::D_ACTIVE, true, "general" );
}

ROI::~ROI()
{
}

PropertyGroup* ROI::properties()
{
    return &m_properties;
}
