/*
 * propertyselection.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: schurade
 */

#include "propertyselection.h"

PropertySelection::PropertySelection( QString name, std::initializer_list<QString>options, int value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new SelectWithLabel( name, 0 );
    int index = 0;
    for ( auto i = options.begin(); i != options.end(); ++i )
    {
        m_widget->insertItem( index++, (*i) );
    }

    m_widget->setCurrentIndex( value );

    connect( m_widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );

}

PropertySelection::PropertySelection( QString name, QVector<QString>options, int value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new SelectWithLabel( name, 0 );
    int index = 0;
    for ( int  i = 0; i < options.size(); ++i )
    {
        m_widget->insertItem( index++, options[i] );
    }

    m_widget->setCurrentIndex( value );

    connect( m_widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );

}
PropertySelection::~PropertySelection()
{
}

QWidget* PropertySelection::getWidget()
{
    return m_widget;
}

QVariant PropertySelection::getValue()
{
    return m_value;
}

void PropertySelection::setValue( QVariant value )
{
    m_value = value.toInt();
    m_widget->setCurrentIndex( m_value );
}

void PropertySelection::widgetChanged( int value )
{
    m_value = value;
    emit( valueChanged() );
    emit( valueChanged( value ) );
}
