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
    ( ( SliderWithEdit* )m_widget )->setValue( m_value.toFloat() );
}

void PropertyFloat::setMin( QVariant min )
{
    m_min = min.toFloat();
    if ( m_value.toFloat() < m_min.toFloat() )
    {
        m_value = m_min;
    }

    ( ( SliderWithEdit* )m_widget )->setMin( m_min.toFloat() );
    ( ( SliderWithEdit* )m_widget )->setValue( m_value.toFloat() );
}

void PropertyFloat::setMax( QVariant max )
{
    m_max = max.toFloat();
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
    m_value = value;
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
