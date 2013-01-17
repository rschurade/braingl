/*
 * propertygroup.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#include "propertygroup.h"

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::~PropertyGroup()
{
}

void PropertyGroup::addProperty( PropertyBool prop )
{
    m_properties[prop.getName()] = &prop;
}

void PropertyGroup::addProperty( PropertyInt prop )
{
    m_properties[prop.getName()] = &prop;
}

void PropertyGroup::addProperty( PropertyFloat prop )
{
    m_properties[prop.getName()] = &prop;
}

void PropertyGroup::addProperty( PropertyString prop )
{
    m_properties[prop.getName()] = &prop;
}

QVariant PropertyGroup::getValue( FN_PROPERTY prop )
{
    return m_properties[prop]->getValue();
}
