/*
 * labelid.cpp
 *
 *  Created on: Aug 16, 2013
 *      Author: schurade
 */

#include "labelid.h"

LabelID::LabelID( int id, QString text, QWidget* parent ) :
    QLabel( text, parent ),
    m_id( id )
{
}

LabelID::~LabelID()
{
}

void LabelID::slotSetHidden( bool state, int id )
{
    if ( id == m_id )
    {
        this->setHidden( state );
    }
}
