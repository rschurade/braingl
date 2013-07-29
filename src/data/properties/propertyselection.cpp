/*
 * propertyselection.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: schurade
 */

#include "propertyselection.h"

PropertySelection::PropertySelection( QString name, std::initializer_list<QString>options, int value ) :
    Property( name, value )
{
    SelectWithLabel* widget = new SelectWithLabel( name, 0 );
    int index = 0;
    for ( auto i = options.begin(); i != options.end(); ++i )
    {
        widget->insertItem( index++, (*i) );
        m_options.push_back( (*i) );
    }

    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;
}

PropertySelection::PropertySelection( QString name, QVector<QString>options, int value ) :
    Property( name, value ),
    m_options( options )
{
    SelectWithLabel* widget = new SelectWithLabel( name, 0 );
    int index = 0;
    for ( int  i = 0; i < options.size(); ++i )
    {
        widget->insertItem( index++, options[i] );
    }

    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;

}
PropertySelection::~PropertySelection()
{
}

void PropertySelection::setValue( QVariant value )
{
    m_value = value;
    ( ( SelectWithLabel* )m_widget )->setCurrentIndex( m_value.toInt() );
}

void PropertySelection::widgetChanged( int value )
{
    m_value = value;
    emit( valueChanged( value ) );
}

QVector<QString> PropertySelection::getOptions()
{
    return m_options;
}
