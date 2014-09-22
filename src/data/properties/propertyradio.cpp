/*
 * propertyradio.cpp
 *
 *  Created on: 21.08.2014
 *      Author: Ralph
 */

#include "propertyradio.h"

PropertyRadio::PropertyRadio( QString name, std::initializer_list<QString> options, int value ) :
    Property( name, value ),
    m_options( options )
{
    RadioGroup* widget = new RadioGroup( name, m_options );
    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;
}

PropertyRadio::PropertyRadio( QString name, std::vector<QString>options, int value ) :
    Property( name, value )
{
    for ( auto i = 0; i < options.size(); ++i )
    {
        m_options.push_back( options[i] );
    }

    RadioGroup* widget = new RadioGroup( name, m_options );
    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;
}

PropertyRadio::PropertyRadio( QString name, QList<QString>options, int value ) :
    Property( name, value ),
    m_options( options )
{
    RadioGroup* widget = new RadioGroup( name, options );
    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;

}

PropertyRadio::~PropertyRadio()
{
}

void PropertyRadio::setValue( QVariant value )
{
    m_value = value;
    ( ( RadioGroup* )m_widget )->setCurrentIndex( m_value.toInt() );
}

void PropertyRadio::widgetChanged( int value )
{
    m_value = value;
    emit( valueChanged( value ) );
}

QList<QString> PropertyRadio::getOptions()
{
    return m_options;
}
