/*
 * propertyfloat.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertyfloat.h"

#include "../../gui/widgets/controls/sliderwithedit.h"

#include <QDebug>

PropertyFloat::PropertyFloat( QString name, float value, float min, float max ) :
    Property( name, value, min, max )
{
    SliderWithEdit* widget = new SliderWithEdit( m_name );
    widget->setMin( min );
    widget->setMax( max );
    widget->setValue( value );
    connect( widget, SIGNAL( valueChanged( float, int ) ), this, SLOT( widgetChanged( float, int ) ) );
    connect( widget, SIGNAL( minChanged( QVariant ) ), this, SLOT( setMin( QVariant ) ) );
    connect( widget, SIGNAL( maxChanged( QVariant ) ), this, SLOT( setMax( QVariant ) ) );
    m_widget = widget;
}

void PropertyFloat::setDigits( int i )
{
    ( (SliderWithEdit*) m_widget )->setDigits( i );
}

PropertyFloat::~PropertyFloat()
{
}

void PropertyFloat::setValue( QVariant value )
{
    m_value = value;
    if ( m_value.toFloat() < m_min.toFloat() )
    {
        setMin( value );
    }
    if ( m_value.toFloat() > m_max.toFloat() )
    {
        setMax( value );
    }
    ( ( SliderWithEdit* )m_widget )->setValue( m_value.toFloat() );
}

void PropertyFloat::setMin( QVariant min )
{
    if ( min.toFloat() >= m_max.toFloat() )
    {
        m_min = m_max.toFloat() - 1;
    }
    else
    {
        m_min = min.toFloat();
    }

    if ( m_value.toFloat() < m_min.toFloat() )
    {
        m_value = m_min;
    }
    ( ( SliderWithEdit* )m_widget )->setDigits( determineDigits() );
    ( ( SliderWithEdit* )m_widget )->setMin( m_min.toFloat() );
    ( ( SliderWithEdit* )m_widget )->setValue( m_value.toFloat() );
}

void PropertyFloat::setMax( QVariant max )
{
    if ( m_max.toFloat() <= m_min.toFloat() )
    {
        m_max = m_min.toFloat() + 1;
    }
    else
    {
        m_max = max.toFloat();
    }
    if ( m_value.toFloat() > m_max.toFloat() )
    {
        m_value = m_max;
    }
    ( ( SliderWithEdit* )m_widget )->setDigits( determineDigits() );
    ( ( SliderWithEdit* )m_widget )->setMax( m_max.toFloat() );
    ( ( SliderWithEdit* )m_widget )->setValue( m_value.toFloat() );
}

void PropertyFloat::widgetChanged( float value, int id )
{
    setValue( value );
    emit( valueChanged( value ) );
}


int PropertyFloat::determineDigits()
{
    int d = 0;

    if ( m_max.toFloat() > m_min.toFloat() )
    {
        float v = m_max.toFloat() - m_min.toFloat();

        while( v < 1 )
        {
            v *= 10;
            ++d;
            if ( v / 10. > 1 )
            {
                break;
            }
        }
        return qMin( 10, qMax( 2, d + 2 ) );
    }
    else
    {
        return 2;
    }
}
