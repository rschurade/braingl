/*
 * globalpropertygroup.cpp
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

bool GlobalPropertyGroup::contains( Fn::Global name ) const
{
    return m_properties.contains( (int)name );
}

QVariant GlobalPropertyGroup::get( Fn::Global name ) const
{
    if ( m_properties.contains( (int)name ) )
    {
        return m_properties[(int)name]->getValue();
    }
    else
    {
        return QVariant();
//        qDebug() << "*** ERROR *** GET" << m_properties[int::NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
//        exit( 0 );
    }
}

bool GlobalPropertyGroup::set( Fn::Global name, QVariant value )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
        return true;
    }
    else
    {
        //qDebug() << "*** ERROR *** SET" << m_properties[int::NAME]->getValue().toString() << Property::getNameAsString( name ) << name << "doesnt exist";
        exit( 0 );
        return false;
    }
}

bool GlobalPropertyGroup::set( Fn::Global name, bool value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( Fn::Global2String::s( (Fn::Global)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, int value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Global2String::s( (Fn::Global)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, int value, int min, int max, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        return false;
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Global2String::s( (Fn::Global)name ), value, min, max );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, float value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( Fn::Global2String::s( (Fn::Global)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, float value, float min, float max, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( Fn::Global2String::s( (Fn::Global)name ), value, min, max );
    m_properties[(int)name] = prop;
    connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    if ( visible )
    {
        m_visible.append( name );
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, QString value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Global2String::s( (Fn::Global)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool GlobalPropertyGroup::set( Fn::Global name, const char* value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Global2String::s( (Fn::Global)name ), QString( value ) );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

QList<Fn::Global> GlobalPropertyGroup::getVisible()
{
    return m_visible;
}

QWidget* GlobalPropertyGroup::getWidget( Fn::Global name )
{
    return m_properties[(int)name]->getWidget();
}

void GlobalPropertyGroup::slotPropChanged()
{
    signalPropChanged();
}

int GlobalPropertyGroup::size() const
{
    return m_properties.size();
}
