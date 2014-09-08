/*
 * propertyint.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertyint.h"

#include "../../gui/widgets/controls/sliderwitheditint.h"

#include <QDebug>

PropertyInt::PropertyInt( QString name, int value, int min, int max ) :
    Property( name, value, min, max )
{
    SliderWithEditInt* widget = new SliderWithEditInt( m_name );
    widget->setMin( min );
    widget->setMax( max );
    widget->setValue( value );
    connect( widget, SIGNAL( valueChanged( int, int ) ), this, SLOT( widgetChanged( int, int ) ) );
    connect( widget, SIGNAL( minChanged( QVariant ) ), this, SLOT( setMin( QVariant ) ) );
    connect( widget, SIGNAL( maxChanged( QVariant ) ), this, SLOT( setMax( QVariant ) ) );
    m_widget = widget;
}

PropertyInt::~PropertyInt()
{
}

void PropertyInt::setValue( QVariant value )
{
    m_value = value.toInt();
    if ( m_value.toInt() < m_min.toInt() )
    {
        setMin( value );
    }
    if ( m_value.toInt() > m_max.toInt() )
    {
        setMax( value );
    }

    ( ( SliderWithEditInt* )m_widget )->setValue( m_value.toInt() );
}

void PropertyInt::setMin( QVariant min )
{
    if ( min.toInt() >= m_max.toInt() )
    {
        m_min = m_max.toInt() - 1;
    }
    else
    {
        m_min = min.toInt();
    }
    if ( m_value.toInt() < m_min.toInt() )
    {
        m_value = m_min;
    }
    ( ( SliderWithEditInt* )m_widget )->setMin( m_min.toInt() );
    ( ( SliderWithEditInt* )m_widget )->setValue( m_value.toInt() );
}

void PropertyInt::setMax( QVariant max )
{
    if ( m_max.toInt() <= m_min.toInt() )
    {
        m_max = m_min.toInt() + 1;
    }
    else
    {
        m_max = max.toInt();
    }
    if ( m_value.toInt() > m_max.toInt() )
    {
        m_value = m_max;
    }
    ( ( SliderWithEditInt* )m_widget )->setMax( m_max.toInt() );
    ( ( SliderWithEditInt* )m_widget )->setValue( m_value.toInt() );
}

void PropertyInt::widgetChanged( int value, int id )
{
    setValue( value );
    emit( valueChanged( value ) );
}
