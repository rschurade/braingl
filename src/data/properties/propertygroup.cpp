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
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            return true;
        }
    }
    return false;
}

QVariant PropertyGroup::get( Fn::Property name ) const
{
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            return m_properties[i].second->getValue();
        }
    }
    return QVariant();
}

bool PropertyGroup::set( Fn::Property name, QVariant value )
{
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            m_properties[i].second->setValue( value );
            return true;
        }
    }

    qDebug() << "*** ERROR *** SET" << "property doesnt exist";
    exit( 0 );
    return false;
}

bool PropertyGroup::set2( Fn::Property name, QVariant value )
{
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            m_properties[i].second->setValue( value );
            return true;
        }
    }

    qDebug() << "*** ERROR *** SET" << "property doesnt exist";
    exit( 0 );
    return false;
}

bool PropertyGroup::set( Fn::Property name, bool value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, int value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, int value, int min, int max, QString tab )
{
    if ( contains( name ) )
    {
        return false;
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, float value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, float value, float min, float max, QString tab )
{
    if ( contains( name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
    prop->setPropertyTab( tab );
    m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
    connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    return true;
}

bool PropertyGroup::set( Fn::Property name, QString value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, const char* value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), QString( value ) );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, QColor value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertyColor* prop = new PropertyColor( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, QDir value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value.absolutePath() );
    }
    else
    {
        PropertyPath* prop = new PropertyPath( Fn::Prop2String::s( name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, std::initializer_list<QString>options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertySelection* prop = new PropertySelection( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::set( Fn::Property name, QVector<QString> options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set2( name, value );
    }
    else
    {
        PropertySelection* prop = new PropertySelection( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged() ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

QWidget* PropertyGroup::getWidget( Fn::Property name )
{
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            return m_properties[i].second->getWidget();
        }
    }
    return 0;
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
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            return m_properties[i].second;
        }
    }
    return 0;
}

Property* PropertyGroup::getNthProperty( int n )
{
    if ( n < 0 || n >= m_properties.size() )
    {
        return 0;
    }
    return m_properties[n].second;
}

bool PropertyGroup::setMinF( Fn::Property name, float value )
{
    if ( contains( name ) )
    {
        ( (PropertyFloat*)( getProperty( name ) ) )->setMin( value );
        return true;
    }
    return false;
}

bool PropertyGroup::setMaxF( Fn::Property name, float value )
{
    if ( contains( name ) )
    {
        ( (PropertyFloat*)( getProperty( name ) ) )->setMax( value );
        return true;
    }
    return false;
}

bool PropertyGroup::setMinI( Fn::Property name, int value )
{
    if ( contains( name ) )
    {
        ( (PropertyInt*)( getProperty( name ) ) )->setMin( value );
        return true;
    }
    return false;
}

bool PropertyGroup::setMaxI( Fn::Property name, int value )
{
    if ( contains( name ) )
    {
        ( (PropertyInt*)( getProperty( name ) ) )->setMax( value );
        return true;
    }
    return false;
}
