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

void PropertyGroup::set( FN_PROPERTY name, bool value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property Bool" << Property::getNameAsString( name );
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << get( FNPROP_NAME ).toString()  << "set Property Bool" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyBool( name, value );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
}

void PropertyGroup::set( FN_PROPERTY name, int value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property Int" << Property::getNameAsString( name );
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property Int" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyInt( name, value );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
}

void PropertyGroup::set( FN_PROPERTY name, float value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property float" << Property::getNameAsString( name );
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property float" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyFloat( name, value );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
}

void PropertyGroup::set( FN_PROPERTY name, QString value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property string" << Property::getNameAsString( name );
        m_properties[name]->setValue( value );
    }
    else
    {
        qDebug() << get( FNPROP_NAME ).toString() << "set Property string" << Property::getNameAsString( name ) << "doesn\'t exist, create new";
        m_properties[name] = new PropertyString( name, value );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
}

QVariant PropertyGroup::getVisible()
{
    return QVariant( m_visible );
}
