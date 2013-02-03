/*
 * propertygroup.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "globalpropertygroup.h"

#include <QtCore/QDebug>

GlobalPropertyGroup::GlobalPropertyGroup()
{
}

GlobalPropertyGroup::~GlobalPropertyGroup()
{
}

bool GlobalPropertyGroup::contains( int name ) const
{
    return m_properties.contains( name );
}

QVariant GlobalPropertyGroup::get( int name ) const
{
    if ( m_properties.contains( name ) )
    {
        return m_properties[name]->getValue();
    }
    else
    {
        return QVariant();
//        qDebug() << "*** ERROR *** GET" << m_properties[int::NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
//        exit( 0 );
    }
}

bool GlobalPropertyGroup::set( int name, QVariant value )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
        return true;
    }
    else
    {
        //qDebug() << "*** ERROR *** SET" << m_properties[int::NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
        exit( 0 );
        return false;
    }
}

bool GlobalPropertyGroup::set( int name, bool value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, int value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, int value, int min, int max, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        return false;
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, float value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, float value, float min, float max, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
    m_properties[name] = prop;
    connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    if ( visible )
    {
        m_visible.append( name );
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, QString value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( value );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( int name, const char* value, bool visible )
{
    if ( m_properties.contains( name ) )
    {
        m_properties[name]->setValue( QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), QString( value ) );
        m_properties[name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

QList<int> GlobalPropertyGroup::getVisible()
{
    return m_visible;
}

QWidget* GlobalPropertyGroup::getWidget( int name )
{
    return m_properties[name]->getWidget();
}

void PropertyGroup::slotPropChanged()
{
    signalPropChanged();
}

int GlobalPropertyGroup::size() const
{
    return m_properties.size();
}
