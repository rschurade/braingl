/*
 * pushbuttonwithid.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: schurade
 */

#include "pushbuttonwithid.h"

PushButtonWithId::PushButtonWithId( const QString& text, int id, QWidget* parent ) :
    QPushButton( text, parent ),
    m_id( id )
{
    connect( this, SIGNAL( clicked() ), this, SLOT(slotClicked() ) );
}

PushButtonWithId::~PushButtonWithId()
{
}

void PushButtonWithId::slotClicked()
{
    emit( signalClicked( m_id ) );
}
