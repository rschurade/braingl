/*
 * comboboxid.cpp
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#include "comboboxid.h"

ComboBoxID::ComboBoxID( int id, QWidget* parent ) :
    QComboBox( parent ),
    m_id( id )
{
    connect( this, SIGNAL( currentIndexChanged( int) ), this, SLOT( slotIndexChanged( int ) ) );
}

ComboBoxID::~ComboBoxID()
{
}

void ComboBoxID::slotIndexChanged( int index )
{
    emit( currentIndexChanged( m_id, index, itemData( index ).toInt() ) );
}

void ComboBoxID::setEnabled2( bool enable, int id )
{
    if ( id == m_id )
    {
        setEnabled( enable );
    }
}
