/*
 * property.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "property.h"

Property::Property( QString name ) :
    m_name( name ),
    m_propertyTab( "none" )
{
}

Property::~Property()
{
}

QString Property::getName()
{
    return m_name;
}

void Property::setPropertyTab( QString tab )
{
    m_propertyTab = tab;
}

QString Property::getPropertyTab()
{
    return m_propertyTab;
}
