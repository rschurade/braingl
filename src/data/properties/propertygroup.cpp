/*
 * propertygroup.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertygroup.h"

#include "propertybool.h"
#include "propertycolor.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertypath.h"
#include "propertystring.h"
#include "propertyselection.h"

#include "../models.h"

#include <QDebug>

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::PropertyGroup( PropertyGroup& pg )
{
    for ( int i = 0; i < pg.size(); ++i )
    {
        QPair<Fn::Property, Property*> pair = pg.getNthPropertyPair( i );
        Property* prop = pair.second;

        if ( dynamic_cast<PropertyBool*>( prop ) )
        {
            create( pair.first, prop->getValue().toBool(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyColor*>( prop ) )
        {
            create( pair.first, prop->getValue().value<QColor>(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyFloat*>( prop ) )
        {
            create( pair.first, prop->getValue().toFloat(), prop->getMin().toFloat(), prop->getMax().toFloat(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyInt*>( prop ) )
        {
            create( pair.first, prop->getValue().toInt(), prop->getMin().toInt(), prop->getMax().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyPath*>( prop ) )
        {
            create( pair.first, QDir( prop->getValue().toString() ), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertySelection*>( prop ) )
        {
            PropertySelection* propSel = dynamic_cast<PropertySelection*>( prop );
            create( pair.first, propSel->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyString*>( prop ) )
        {
            create( pair.first, prop->getValue().toString(), prop->getPropertyTab() );
        }
    }
}

PropertyGroup::~PropertyGroup()
{
}

bool PropertyGroup::create( Fn::Property name, bool value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyBool* prop = new PropertyBool( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyInt* prop = new PropertyInt( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, int value, int min, int max, QString tab )
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
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, float value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, float value, float min, float max, QString tab )
{
    if ( contains( name ) )
    {
        return false;
    }
    PropertyFloat* prop = new PropertyFloat( Fn::Prop2String::s( (Fn::Property)name ), value, min, max );
    prop->setPropertyTab( tab );
    m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
    connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    return true;
}

bool PropertyGroup::create( Fn::Property name, QString value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, const char* value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, QString( value ) );
    }
    else
    {
        PropertyString* prop = new PropertyString( Fn::Prop2String::s( (Fn::Property)name ), QString( value ) );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, QColor value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyColor* prop = new PropertyColor( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, QDir value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value.absolutePath() );
    }
    else
    {
        PropertyPath* prop = new PropertyPath( Fn::Prop2String::s( name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, std::initializer_list<QString>options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertySelection* prop = new PropertySelection( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::create( Fn::Property name, QVector<QString> options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertySelection* prop = new PropertySelection( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
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

    QString propName("");
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == Fn::Property::D_NAME )
        {
            propName = m_properties[i].second->getValue().toString();
        }
    }

    qDebug() << "*** ERROR *** SET" << "property doesnt exist:" << Fn::Prop2String::s( name ) << propName;
    exit( 0 );
    return false;
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
    Models::d()->submit();
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
        qDebug() << "***ERROR*** getNthProperty index out of range";
        exit( 0 );
    }
    return m_properties[n].second;
}

QPair<Fn::Property, Property*> PropertyGroup::getNthPropertyPair( int n )
{
    if ( n < 0 || n >= m_properties.size() )
    {
        qDebug() << "***ERROR*** getNthPropertyPair index out of range";
        exit( 0 );
    }
    return m_properties[n];
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

QList<QVariant>PropertyGroup::getState()
{
    QList<QVariant> state;
    for ( int i = 0; i < m_properties.size(); ++i )
    {
        state.push_back( (int)m_properties[i].first );
        state.push_back( m_properties[i].second->getValue() );
    }

    return state;
}

void PropertyGroup::setState( QList<QVariant> state )
{
    for ( int i = 0; i < state.size() / 2; ++i )
    {
        set( (Fn::Property)( state[i*2].toInt() ), state[i*2+1] );
    }
}
