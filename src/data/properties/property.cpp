/*
 * property.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "property.h"

Property::Property( Fn::Property name ) :
    m_name( name ),
    m_label( Fn::Property2String::s( name ) )
{
}

Property::~Property()
{
}

Fn::Property Property::getName()
{
    return m_name;
}

QString Property::getLabel()
{
    return m_label;
}

