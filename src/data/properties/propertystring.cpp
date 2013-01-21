/*
 * Propertystring.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "../../gui/widgets/editwithlabel.h"

#include "propertystring.h"

PropertyString::PropertyString( FN_PROPERTY name, QString value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new EditWithLabel( m_label );
    connect( m_widget, SIGNAL( valueChanged( QString, int ) ), this, SLOT( widgetChanged( QString, int ) ) );
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
