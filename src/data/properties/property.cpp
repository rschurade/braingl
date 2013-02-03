/*
 * property.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "property.h"

Property::Property( QString name ) :
    m_name( name )
{
}

Property::~Property()
{
}

QString Property::getName()
{
    return m_name;
}

