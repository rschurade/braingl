/*
 * Propertystring.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "propertystring.h"

#include "../../gui/widgets/controls/editwithlabel.h"

PropertyString::PropertyString( QString name, QString value ) :
    Property( name, value )
{
    EditWithLabel* widget = new EditWithLabel( m_name );
    widget->setText( value );
    connect( widget, SIGNAL( valueChanged( QString, int ) ), this, SLOT( widgetChanged( QString, int ) ) );
    m_widget = widget;
}

PropertyString::~PropertyString()
{
}

void PropertyString::setValue( QVariant value )
{
    m_value = value;
}

void PropertyString::widgetChanged( QString value, int id )
{
    m_value = value;
    emit( valueChanged( value ) );
}
