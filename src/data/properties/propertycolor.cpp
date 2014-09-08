/*
 * propertycolor.cpp
 *
 * Created on: Feb 4, 2013
 * @author Ralph Schurade
 */

#include "propertycolor.h"

#include "../../gui/widgets/controls/colorwidgetwithlabel.h"

PropertyColor::PropertyColor( QString name, QColor value ) :
    Property( name, value )
{
    ColorWidgetWithLabel* widget = new ColorWidgetWithLabel( m_name );
    widget->setValue( value );
    connect( widget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( widgetChanged( QColor, int ) ) );
    m_widget = widget;
}

PropertyColor::~PropertyColor()
{
}

void PropertyColor::setValue( QVariant value )
{
    m_value = value;
    ColorWidgetWithLabel* widget = dynamic_cast<ColorWidgetWithLabel*>( m_widget );
    widget->setValue( value.value<QColor>() );
}

void PropertyColor::widgetChanged( QColor value, int id )
{
    m_value = value;
    emit( valueChanged( value ) );
}
