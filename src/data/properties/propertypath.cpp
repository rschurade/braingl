/*
 * propertypath.cpp
 *
 * Created on: Mar 6, 2013
 * @author Ralph Schurade
 */

#include "propertypath.h"

#include "../../gui/widgets/controls/pathwidgetwithlabel.h"

PropertyPath::PropertyPath( QString name, QDir value ) :
    Property( name ),
    m_value( value )
{
    m_widget = new PathWidgetWithLabel( m_name );
    m_widget->setValue( value );
    connect( m_widget, SIGNAL( valueChanged( QDir, int ) ), this, SLOT( widgetChanged( QDir, int ) ) );
}

PropertyPath::~PropertyPath()
{
}

QWidget* PropertyPath::getWidget()
{
    return m_widget;
}

QVariant PropertyPath::getValue()
{
    return m_value.absolutePath();
}

void PropertyPath::setValue( QVariant value )
{
    m_value = QDir( value.toString() );
    m_widget->setValue( m_value );
}

void PropertyPath::setValue( QDir value )
{
    m_value = value;
    m_widget->setValue( m_value );
}

void PropertyPath::widgetChanged( QDir value, int id )
{
    m_value = value;
    emit( valueChanged() );
    emit( pathChanged( m_value ) );
}
