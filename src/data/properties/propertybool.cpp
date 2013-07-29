/*
 * PropertyBool.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertybool.h"

#include "../../gui/widgets/controls/checkboxwithlabel.h"

#include <QDebug>

PropertyBool::PropertyBool( QString name, bool value ) :
    Property( name, value )
{
    CheckboxWithLabel* widget = new CheckboxWithLabel( m_name );
    widget->setChecked( value );
    connect( widget, SIGNAL( stateChanged( int, int ) ), this, SLOT( widgetChanged( int, int ) ) );
    m_widget = widget;
}

PropertyBool::~PropertyBool()
{
}


void PropertyBool::setValue( QVariant value )
{
    m_value = value;
    ( ( CheckboxWithLabel* )m_widget )->setChecked( m_value.toBool() );
}

void PropertyBool::widgetChanged( int value, int id )
{
    m_value = value;
    if ( m_widget->isVisible() )
    {
        emit( valueChanged( value ) );
    }
}
