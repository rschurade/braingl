/*
 * propertygroup.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "propertygroup.h"

#include <QtCore/QDebug>

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::~PropertyGroup()
{
}

bool PropertyGroup::contains( FN_PROPERTY name ) const
{
    return m_properties.contains( name );
}

QVariant PropertyGroup::get( FN_PROPERTY name ) const
{
    if ( m_properties.contains( name ) )
    {
        return m_properties[name]->getValue();
    }
    else
    {
        return QVariant();
//        qDebug() << "*** ERROR *** GET" << m_properties[FNPROP_NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
//        exit( 0 );
    }
}

bool PropertyGroup::set( FN_PROPERTY name, QVariant value )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
        return true;
    }
    else
    {
        qDebug() << "*** ERROR *** SET" << m_properties[FNPROP_NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
        exit( 0 );
        return false;
    }
}

bool PropertyGroup::set( FN_PROPERTY name, bool value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( name, value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, int value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( name, value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, int value, int min, int max, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        return false;
    }
    else
    {
        PropertyInt* prop = new PropertyInt( name, value, min, max );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, float value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( name, value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, float value, float min, float max, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( name, value, min, max );
    m_properties[name] = prop;
    connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    if ( visible )
    {
        m_visible.append( name );
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, QString value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyString* prop = new PropertyString( name, value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( FN_PROPERTY name, const char* value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( name, QString( value ) );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

QList<FN_PROPERTY> PropertyGroup::getVisible()
{
    return m_visible;
}

QWidget* PropertyGroup::getWidget( FN_PROPERTY name )
{
    return m_properties[name]->getWidget();
}

void PropertyGroup::slotPropChanged()
{
    signalPropChanged();
}

int PropertyGroup::size() const
{
    return m_properties.size();
}
