/*
 * property.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#include "property.h"

Property::Property( FN_PROPERTY name, QString label ) :
    m_name( name ),
    m_label( label )
{
}

Property::~Property()
{
}

FN_PROPERTY Property::getName()
{
    return m_name;
}

QString Property::getLabel()
{
    return m_label;
}
