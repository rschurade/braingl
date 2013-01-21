/*
 * propertygroup.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include "propertygroup.h"

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::~PropertyGroup()
{
}

QVariant PropertyGroup::getValue( FN_PROPERTY prop )
{
    return m_properties[prop]->getValue();
}

bool PropertyGroup::contains( FN_PROPERTY name )
{
    return m_properties.contains( name );
}

QVariant PropertyGroup::get( FN_PROPERTY name )
{
    if ( m_properties.contains( name ) )
    {
        return m_properties[name]->getValue();
    }
    else
    {
        return QVariant();
    }
}

void PropertyGroup::set( FN_PROPERTY name, bool value )
{
    qDebug() << "set Property Bool" << Property::getNameAsString( name );
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << "set Property Bool" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyBool( name, value );
    }
}

void PropertyGroup::set( FN_PROPERTY name, int value )
{
    qDebug() << "set Property Int" << Property::getNameAsString( name );
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << "set Property Int" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyInt( name, value );
    }
}

void PropertyGroup::set( FN_PROPERTY name, float value )
{
    qDebug() << "set Property float" << Property::getNameAsString( name );
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << "set Property float" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyFloat( name, value );
    }
}

void PropertyGroup::set( FN_PROPERTY name, QString value )
{
    qDebug() << "set Property string" << Property::getNameAsString( name );
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << "set Property string" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyString( name, value );
    }
}

QHash<FN_PROPERTY, Property*> PropertyGroup::getAll()
{
    return m_properties;
}
