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

PropertySelection::PropertySelection( QString name, std::vector<QString>options, int value ) :
    Property( name, value ),
    m_options( options )
{
    SelectWithLabel* widget = new SelectWithLabel( name, 0 );
    int index = 0;
    for ( unsigned int  i = 0; i < options.size(); ++i )
    {
        widget->insertItem( index++, options[i] );
    }

    widget->setCurrentIndex( value );

    connect( widget, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( widgetChanged( int) ) );
    m_widget = widget;

}

PropertySelection::PropertySelection( QString name, QList<QString>options, int value ) :
    Property( name, value )
{
    for ( int i = 0; i < options.size(); ++i )
    {
        m_options.push_back( options[i] );
    }

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

std::vector<QString> PropertySelection::getOptions()
{
    return m_options;
}

void PropertySelection::clear()
{
    m_options.clear();
    static_cast<SelectWithLabel*>( m_widget )->clear();
}

void PropertySelection::setOptions( std::initializer_list<QString> options )
{
    SelectWithLabel* widget = static_cast<SelectWithLabel*>( m_widget );
    int index = 0;
    widget->blockSignals( true );
    for ( auto i = options.begin(); i != options.end(); ++i )
    {
        widget->insertItem( index++, (*i) );
        m_options.push_back( (*i) );
    }
    widget->blockSignals( false );
}

void PropertySelection::addOption( QString option )
{
    SelectWithLabel* widget = static_cast<SelectWithLabel*>( m_widget );
    widget->blockSignals( true );
    widget->addItem( option );
    m_options.push_back( ( option ) );
    widget->blockSignals( false );
}
