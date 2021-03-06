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
#include "propertytext.h"
#include "propertyradio.h"
#include "propertyselection.h"
#include "propertybutton.h"
#include "propertymatrix.h"
#include "propertyvector.h"

#include "../models.h"

#include <QDebug>

PropertyGroup::PropertyGroup()
{
}

PropertyGroup::PropertyGroup( const PropertyGroup& pg )
{
    for ( int i = 0; i < pg.size(); ++i )
    {
        QPair<Fn::Property, Property*> pair = pg.getNthPropertyPair( i );
        Property* prop = pair.second;

        if ( dynamic_cast<PropertyBool*>( prop ) )
        {
            createBool( pair.first, prop->getValue().toBool(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyColor*>( prop ) )
        {
            createColor( pair.first, prop->getValue().value<QColor>(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyFloat*>( prop ) )
        {
            createFloat( pair.first, prop->getValue().toFloat(), prop->getMin().toFloat(), prop->getMax().toFloat(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyInt*>( prop ) )
        {
            createInt( pair.first, prop->getValue().toInt(), prop->getMin().toInt(), prop->getMax().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyPath*>( prop ) )
        {
            createDir( pair.first, QDir( prop->getValue().toString() ), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertySelection*>( prop ) )
        {
            PropertySelection* propSel = dynamic_cast<PropertySelection*>( prop );
            createList( pair.first, propSel->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyRadio*>( prop ) )
        {
            PropertyRadio* propRad = dynamic_cast<PropertyRadio*>( prop );
            createRadioGroup( pair.first, propRad->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyString*>( prop ) )
        {
            createString( pair.first, prop->getValue().toString(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyText*>( prop ) )
        {
            createText( pair.first, prop->getValue().toString(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyMatrix*>( prop ) )
        {
            createMatrix( pair.first, prop->getValue().value<QMatrix4x4>(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyVector*>( prop ) )
        {
            createVector( pair.first, prop->getValue().value<QVector3D>(), prop->getPropertyTab() );
        }
    }
}

PropertyGroup::~PropertyGroup()
{
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
    {
        delete m_properties[i].second;
    }
}

PropertyGroup& PropertyGroup::operator=( const PropertyGroup& pg )
{
    for ( int i = 0; i < pg.size(); ++i )
    {
        QPair<Fn::Property, Property*> pair = pg.getNthPropertyPair( i );
        Property* prop = pair.second;

        if ( dynamic_cast<PropertyBool*>( prop ) )
        {
            this->createBool( pair.first, prop->getValue().toBool(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyColor*>( prop ) )
        {
            this->createColor( pair.first, prop->getValue().value<QColor>(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyFloat*>( prop ) )
        {
            this->createFloat( pair.first, prop->getValue().toFloat(), prop->getMin().toFloat(), prop->getMax().toFloat(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyInt*>( prop ) )
        {
            this->createInt( pair.first, prop->getValue().toInt(), prop->getMin().toInt(), prop->getMax().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyPath*>( prop ) )
        {
            this->createDir( pair.first, QDir( prop->getValue().toString() ), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertySelection*>( prop ) )
        {
            PropertySelection* propSel = dynamic_cast<PropertySelection*>( prop );
            this->createList( pair.first, propSel->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyString*>( prop ) )
        {
            this->createString( pair.first, prop->getValue().toString(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyText*>( prop ) )
        {
            this->createText( pair.first, prop->getValue().toString(), prop->getPropertyTab() );
        }
        if ( dynamic_cast<PropertyRadio*>( prop ) )
        {
            PropertyRadio* propRad = dynamic_cast<PropertyRadio*>( prop );
            createRadioGroup( pair.first, propRad->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
        }
    }
    return *this;
}

void PropertyGroup::copy( Fn::Property name, Property* prop )
{
	if ( dynamic_cast<PropertyBool*>( prop ) )
	{
		this->createBool( name, prop->getValue().toBool(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyColor*>( prop ) )
	{
		this->createColor( name, prop->getValue().value<QColor>(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyFloat*>( prop ) )
	{
		this->createFloat( name, prop->getValue().toFloat(), prop->getMin().toFloat(), prop->getMax().toFloat(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyInt*>( prop ) )
	{
		this->createInt( name, prop->getValue().toInt(), prop->getMin().toInt(), prop->getMax().toInt(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyPath*>( prop ) )
	{
		this->createDir( name, QDir( prop->getValue().toString() ), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertySelection*>( prop ) )
	{
		PropertySelection* propSel = dynamic_cast<PropertySelection*>( prop );
		this->createList( name, propSel->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyRadio*>( prop ) )
    {
        PropertyRadio* propRad = dynamic_cast<PropertyRadio*>( prop );
        createRadioGroup( name, propRad->getOptions(), prop->getValue().toInt(), prop->getPropertyTab() );
    }
	if ( dynamic_cast<PropertyString*>( prop ) )
	{
		this->createString( name, prop->getValue().toString(), prop->getPropertyTab() );
	}
	if ( dynamic_cast<PropertyText*>( prop ) )
	{
		this->createText( name, prop->getValue().toString(), prop->getPropertyTab() );
	}
}

bool PropertyGroup::createBool( Fn::Property name, bool value, QString tab )
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

bool PropertyGroup::createInt( Fn::Property name, int value, QString tab )
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

bool PropertyGroup::createInt( Fn::Property name, int value, int min, int max, QString tab )
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

bool PropertyGroup::createFloat( Fn::Property name, float value, QString tab )
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

bool PropertyGroup::createFloat( Fn::Property name, float value, float min, float max, QString tab )
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

bool PropertyGroup::createString( Fn::Property name, QString value, QString tab )
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

bool PropertyGroup::createCharString( Fn::Property name, const char* value, QString tab )
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

bool PropertyGroup::createText( Fn::Property name, QString value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyText* prop = new PropertyText( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::createColor( Fn::Property name, QColor value, QString tab )
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

bool PropertyGroup::createMatrix( Fn::Property name, QMatrix4x4 value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyMatrix* prop = new PropertyMatrix( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::createVector( Fn::Property name, QVector3D value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyVector* prop = new PropertyVector( Fn::Prop2String::s( (Fn::Property)name ), value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}


bool PropertyGroup::createDir( Fn::Property name, QDir value, QString tab )
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

bool PropertyGroup::createList( Fn::Property name, std::initializer_list<QString>options, int value, QString tab )
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

bool PropertyGroup::createList( Fn::Property name, std::vector<QString> options, int value, QString tab )
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

bool PropertyGroup::createList( Fn::Property name, QList<QString> options, int value, QString tab )
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

bool PropertyGroup::createRadioGroup( Fn::Property name, std::initializer_list<QString>options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyRadio* prop = new PropertyRadio( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::createRadioGroup( Fn::Property name, std::vector<QString> options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyRadio* prop = new PropertyRadio( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::createRadioGroup( Fn::Property name, QList<QString> options, int value, QString tab )
{
    if ( contains( name ) )
    {
        set( name, value );
    }
    else
    {
        PropertyRadio* prop = new PropertyRadio( Fn::Prop2String::s( (Fn::Property)name ), options, value );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}


bool PropertyGroup::createButton( Fn::Property name, QString tab )
{
    if ( contains( name ) )
    {
        return true;
    }
    else
    {
        PropertyButton* prop = new PropertyButton( Fn::Prop2String::s( (Fn::Property)name ) );
        prop->setPropertyTab( tab );
        m_properties.push_back( QPair<Fn::Property, Property*>( name, prop ) );
        connect( prop, SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotPropChanged() ) );
    }
    return true;
}

bool PropertyGroup::contains( Fn::Property name ) const
{
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
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
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
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
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == name )
        {
            m_properties[i].second->setValue( value );
            emit( signalSetProp( (int) name ) );
            return true;
        }
    }

    QString propName("");
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].first == Fn::Property::D_NAME )
        {
            propName = m_properties[i].second->getValue().toString();
        }
    }

    qCritical() << "*** ERROR *** SET" << "property doesnt exist:" << Fn::Prop2String::s( name ) << propName;
//    exit( 0 );
    return false;
}

QWidget* PropertyGroup::getWidget( Fn::Property name )
{
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
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
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
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
    if ( n < 0 || n >= (int)m_properties.size() )
    {
        qCritical() << "***ERROR*** getNthProperty index out of range";
        exit( 0 );
    }
    return m_properties[n].second;
}

QPair<Fn::Property, Property*> PropertyGroup::getNthPropertyPair( int n ) const
{
    if ( n < 0 || n >= (int)m_properties.size() )
    {
        qCritical() << "***ERROR*** getNthPropertyPair index out of range";
        exit( 0 );
    }
    return m_properties[n];
}

bool PropertyGroup::setMin( Fn::Property name, QVariant value )
{
    if ( contains( name ) )
    {
        ( (Property*)( getProperty( name ) ) )->setMin( value );
        return true;
    }
    return false;
}

bool PropertyGroup::setMax( Fn::Property name, QVariant value )
{
    if ( contains( name ) )
    {
        ( (Property*)( getProperty( name ) ) )->setMax( value );
        return true;
    }
    return false;
}

QList<QVariant>PropertyGroup::getState()
{
    QList<QVariant> state;
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
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

void PropertyGroup::unsetTab( QString tab )
{
    for ( unsigned int i = 0; i < m_properties.size(); ++i )
    {
        if ( m_properties[i].second->getPropertyTab() == tab )
        {
            m_properties[i].second->setPropertyTab( "none" );
        }
    }
}
