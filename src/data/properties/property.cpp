/*
 * property.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "property.h"

#include <QDebug>
#include <QWidget>

Property::Property( QString name, QVariant value, QVariant min, QVariant max ) :
    m_widget( 0 ),
    m_name( name ),
    m_propertyTab( "none" ),
    m_value( value ),
    m_min( min ),
    m_max( max )

{
}

Property::~Property()
{
    delete m_widget;
}

QString Property::getName()
{
    return m_name;
}

QWidget* Property::getWidget()
{
    return m_widget;
}

void Property::setPropertyTab( QString tab )
{
    m_propertyTab = tab;
}

QString Property::getPropertyTab()
{
    return m_propertyTab;
}

QVariant Property::getValue()
{
    return m_value;
}

QVariant Property::getMin()
{
    return m_min;
}

QVariant Property::getMax()
{
    return m_max;
}

void Property::setMin( QVariant value )
{

}

void Property::setMax( QVariant value )
{

}
