/*
 * Propertystring.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "../../gui/widgets/checkboxwithlabel.h"

#include "propertystring.h"

PropertyString::PropertyString( FN_PROPERTY name, QString value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new CheckboxWithLabel( m_label );
    connect( m_widget, SIGNAL( stateChanged( int, int ) ), this, SLOT( widgetChanged( int, int ) ) );
}

PropertyString::~PropertyString()
{
}

QWidget* PropertyString::getWidget()
{
    return m_widget;
}

QVariant PropertyString::getValue()
{
    return m_value;
}

void PropertyString::setValue( QVariant value )
{
    m_value = value.toString();
}

void PropertyString::widgetChanged( QString value, int id )
{
    emit( valueChanged() );
}
