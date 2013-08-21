/*
 * propertybutton.cpp
 *
 *  Created on: Aug 20, 2013
 *      Author: schurade
 */

#include "propertybutton.h"

#include "../../gui/widgets/controls/buttonwithlabel.h"

PropertyButton::PropertyButton( QString name ) :
    Property( name, false )
{
    ButtonWithLabel* widget = new ButtonWithLabel( m_name );
    connect( widget, SIGNAL( buttonPressed( int ) ), this, SLOT( widgetChanged( int ) ) );
    m_widget = widget;

}

PropertyButton::~PropertyButton()
{

}

void PropertyButton::widgetChanged( int id )
{
    {
        emit( valueChanged( true ) );
    }
}
