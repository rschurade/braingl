/*
 * propertyint.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
 */
#include "propertyint.h"

#include "../../gui/widgets/controls/sliderwitheditint.h"

#include <QtCore/QDebug>

PropertyInt::PropertyInt( QString name, int value, int min, int max ) :
    Property( name ),
    m_value( value ),
    m_min( min ),
    m_max( max )
{
    m_widget = new SliderWithEditInt( m_name );
    m_widget->setMin( min );
    m_widget->setMax( max );
    m_widget->setValue( value );
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
    if ( m_value > m_max )
    {
        m_max = m_value;
        m_widget->setMax( m_max );
    }
    m_widget->setValue( m_value );
}

void PropertyInt::setMin( int min )
{
    m_min = min;
    if ( m_value < m_min )
    {
        m_value = m_min;
    }
    m_widget->setMin( min );
    m_widget->setValue( m_value );
}

void PropertyInt::setMax( int max )
{
    m_max = max;
    if ( m_value > m_max )
    {
        m_value = m_max;
    }
    m_widget->setMax( max );
    m_widget->setValue( m_value );
}

void PropertyInt::widgetChanged( int value, int id )
{
    m_value = value;
    emit( valueChanged() );
    emit( valueChanged( value ) );
}
