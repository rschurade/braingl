/*
 * propertygroup.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertygroup.h"

#include <QDebug>

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::~PropertyGroup()
{
}

bool PropertyGroup::contains( Fn::Property name ) const
{
    return m_properties.contains( (int)name );
}

QVariant PropertyGroup::get( Fn::Property name ) const
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

bool PropertyGroup::set( Fn::Property name, QVariant value )
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

bool PropertyGroup::set( Fn::Property name, bool value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, int value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, int value, int min, int max, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        return false;
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, float value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, float value, float min, float max, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
    m_properties[(int)name] = prop;
    connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    if ( visible )
    {
        m_visible.append( name );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, QString value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, const char* value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), QString( value ) );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, QColor value, bool visible )
{
    if ( m_properties.contains( (int)name ) )
    {
        m_properties[(int)name]->setValue( value );
    }
    else
    {
        PropertyColor* prop = new PropertyColor( Fn::Prop2String::s( (Fn::Property)name ), value );
        m_properties[(int)name] = prop;
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
        if ( visible )
        {
            m_visible.append( name );
        }
    }
    return true;
}

QList<Fn::Property> PropertyGroup::getVisible()
{
    return m_visible;
}

QWidget* PropertyGroup::getWidget( Fn::Property name )
{
    return m_properties[(int)name]->getWidget();
}

void PropertyGroup::slotPropChanged()
{
    emit( signalPropChanged() );
}

int PropertyGroup::size() const
{
    return m_properties.size();
}

Property* PropertyGroup::getProperty( Fn::Property name )
{
    if ( m_properties.contains( (int)name ) )
    {
        return m_properties[(int)name];
    }
    else
    {
        return 0;
    }
}
