/*
 * propertycolor.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: Ralph Schurade
 */

#include "propertycolor.h"

#include "../../gui/widgets/controls/colorwidgetwithlabel.h"

PropertyColor::PropertyColor( QString name, QColor value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new ColorWidgetWithLabel( m_name );
    m_widget->setValue( value );
    connect( m_widget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( widgetChanged( QColor, int ) ) );
}

PropertyColor::~PropertyColor()
{
}

QWidget* PropertyColor::getWidget()
{
    return m_widget;
}

QVariant PropertyColor::getValue()
{
    return QVariant::fromValue( m_value );
}

void PropertyColor::setValue( QVariant value )
{
    m_value = value.value<QColor>();
    m_widget->setValue( m_value );
}

void PropertyColor::setValue( QColor value )
{
    m_value = value;
    m_widget->setValue( m_value );
}

void PropertyColor::widgetChanged( QColor value, int id )
{
    m_value = value;
    emit( valueChanged() );
    emit( colorChanged( m_value ) );
}
