/*
 * PropertyBool.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "propertybool.h"

#include "../../gui/widgets/controls/checkboxwithlabel.h"

PropertyBool::PropertyBool( QString name, bool value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new CheckboxWithLabel( m_name );
    m_widget->setChecked( value );
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
    m_widget->setChecked( m_value );
}

void PropertyBool::widgetChanged( int value, int id )
{
    m_value = value;
    emit( valueChanged() );
}
