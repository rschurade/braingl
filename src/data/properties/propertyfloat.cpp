/*
 * propertyfloat.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "propertyfloat.h"

#include "../../gui/widgets/controls/sliderwithedit.h"

PropertyFloat::PropertyFloat( QString name, float value, float min, float max ) :
    Property( name ),
    m_value( value ),
    m_min( min ),
    m_max( max )
{
    m_widget = new SliderWithEdit( m_name );
    m_widget->setMin( min );
    m_widget->setMax( max );
    m_widget->setValue( value );
    connect( m_widget, SIGNAL( valueChanged( float, int ) ), this, SLOT( widgetChanged( float, int ) ) );
}

PropertyFloat::~PropertyFloat()
{
}

QWidget* PropertyFloat::getWidget()
{
    return m_widget;
}

QVariant PropertyFloat::getValue()
{
    return m_value;
}

void PropertyFloat::setValue( QVariant value )
{
    m_value = value.toFloat();
}

void PropertyFloat::setMin( float min )
{
    m_min = min;
    if ( m_value < m_min )
    {
        m_value = m_min;
    }
    m_widget->setMin( min );
    m_widget->setValue( m_value );
}

void PropertyFloat::setMax( float max )
{
    m_max = max;
    if ( m_value > m_max )
    {
        m_value = m_max;
    }
    m_widget->setMax( max );
    m_widget->setValue( m_value );
}

void PropertyFloat::widgetChanged( float value, int id )
{
    m_value = value;
    emit( valueChanged() );
}
