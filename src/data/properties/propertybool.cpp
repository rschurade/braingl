/*
 * PropertyBool.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "../../gui/widgets/checkboxwithlabel.h"

#include "propertybool.h"

PropertyBool::PropertyBool( FN_PROPERTY name, bool value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new CheckboxWithLabel( m_label );
    connect( m_widget, SIGNAL( stateChanged( int, int ) ), this, SLOT( widgetChanged( int, int ) ) );
}

PropertyBool::~PropertyBool()
{
}

QWidget* PropertyBool::getWidget()
{
    return m_widget;
}

QVariant PropertyBool::getValue()
{
    return m_value;
}

void PropertyBool::setValue( QVariant value )
{
    m_value = value.toBool();
}

void PropertyBool::widgetChanged( int value, int id )
{
    emit( valueChanged() );
}
