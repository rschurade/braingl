/*
 * checkboxid.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: schurade
 */

#include "checkboxid.h"

CheckBoxID::CheckBoxID( int id, QWidget* parent ) :
    QCheckBox( parent ),
    m_id( id )
{
    connect( this, SIGNAL( stateChanged( int) ), this, SLOT( slotStateChanged( int ) ) );

}

CheckBoxID::~CheckBoxID()
{
}

void CheckBoxID::slotStateChanged( int state )
{
    emit( signalStateChanged( m_id, state ) );
}
