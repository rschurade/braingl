/*
 * propertyfloat.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include <QtGui/QWidget>

#include "../../gui/widgets/sliderwithedit.h"

#include "propertyfloat.h"

PropertyFloat::PropertyFloat( FN_PROPERTY name, float value, float min, float max ) :
    Property( name ),
    m_value( value ),
    m_min( min ),
    m_max( max )
{
    m_widget = new SliderWithEdit( m_label );
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
}

void PropertyFloat::setMax( float max )
{
    m_max = max;
    if ( m_value > m_max )
    {
        m_value = m_max;
    }
}

void PropertyFloat::widgetChanged( float value, int id )
{
    emit( valueChanged() );
}
