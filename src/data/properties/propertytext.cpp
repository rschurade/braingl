/*
 * propertytext.cpp
 *
 * Created on: Nov 15, 2013
 * @author Ralph Schurade
 */
#include "propertytext.h"

#include "../../gui/widgets/controls/texteditwithlabel.h"

#include <QDebug>

PropertyText::PropertyText( QString name, QString value ) :
    Property( name, value )
{
    qDebug() << value;
    TextEditWithLabel* widget = new TextEditWithLabel( m_name );
    widget->setText( value );
    connect( widget, SIGNAL( valueChanged( QString, int ) ), this, SLOT( widgetChanged( QString, int ) ) );
    m_widget = widget;
}

PropertyText::~PropertyText()
{
}

void PropertyText::setValue( QVariant value )
{
    m_value = value;
    dynamic_cast<TextEditWithLabel*>( m_widget )->setText( value.toString() );
}

void PropertyText::widgetChanged( QString value, int id )
{
    m_value = value;
    emit( valueChanged( value ) );
}
