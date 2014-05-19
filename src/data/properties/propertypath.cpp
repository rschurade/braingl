/*
 * propertypath.cpp
 *
 * Created on: Mar 6, 2013
 * @author Ralph Schurade
 */

#include "propertypath.h"

#include "../../gui/widgets/controls/pathwidgetwithlabel.h"

PropertyPath::PropertyPath( QString name, QDir value ) :
    Property( name, value.absolutePath() )
{
    PathWidgetWithLabel* widget = new PathWidgetWithLabel( m_name );
    widget->setValue( value );
    connect( widget, SIGNAL( valueChanged( QDir, int ) ), this, SLOT( widgetChanged( QDir, int ) ) );
    m_widget = widget;
}

PropertyPath::~PropertyPath()
{
}

void PropertyPath::setValue( QVariant value )
{
    m_value = value;
    PathWidgetWithLabel* widget = dynamic_cast<PathWidgetWithLabel*>( m_widget );
    widget->setValue( value.toString() );
}

void PropertyPath::widgetChanged( QDir value, int id )
{
    m_value = value.absolutePath();
    emit( valueChanged( value.absolutePath() ) );
}
