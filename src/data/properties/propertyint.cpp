/*
 * propertyint.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "../../gui/widgets/sliderwitheditint.h"

#include "propertyint.h"

PropertyInt::PropertyInt( FN_PROPERTY name, QString label, int value, int min, int max ) :
    Property( name, label ),
    m_value( value ),
    m_min( min ),
    m_max( max )
{
    m_widget = new SliderWithEditInt( label );
    connect( m_widget, SIGNAL( valueChanged( int, int ) ), this, SLOT( widgetChanged( int, int ) ) );
}

PropertyInt::~PropertyInt()
{
}

QWidget* PropertyInt::getWidget()
{
    return m_widget;
}

QVariant PropertyInt::getValue()
{
    return m_value;
}

void PropertyInt::setValue( QVariant value )
{
    m_value = value.toInt();
}

void PropertyInt::setMin( int min )
{
    m_min = min;
    if ( m_value < m_min )
    {
        m_value = m_min;
    }
}

void PropertyInt::setMax( int max )
{
    m_max = max;
    if ( m_value > m_max )
    {
        m_value = m_max;
    }
}

void PropertyInt::widgetChanged( int value, int id )
{
    emit( valueChanged() );
}
