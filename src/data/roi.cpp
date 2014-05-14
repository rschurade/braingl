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
    m_properties.createString( Fn::Property::D_NAME, name, "general" );
    m_properties.createBool( Fn::Property::D_ACTIVE, true, "general" );
    m_properties.createInt( Fn::Property::D_ID, m_count );
    m_properties.createText( Fn::Property::D_DESCRIPTION, QString(""), "description" );

    connect( &m_properties, SIGNAL( signalPropChanged() ), this, SLOT( slotPropChanged() ) );
}

ROI::~ROI()
{
}

PropertyGroup* ROI::properties()
{
    return &m_properties;
}

void ROI::slotPropChanged()
{
    emit ( signalPropChanged( m_properties.get( Fn::Property::D_ID ).toInt() ) );
}
