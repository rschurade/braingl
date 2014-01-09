/*
 * lineeditid.cpp
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#include "lineeditid.h"

LineEditID::LineEditID( int id, QWidget* parent ) :
    QLineEdit( parent ),
    m_id( id )
{
    connect( this, SIGNAL( textEdited( QString ) ), this, SLOT( slotTextEdited( QString ) ) );
}

LineEditID::~LineEditID()
{
}

void LineEditID::setEnabled2( bool enable, int id )
{
    if ( id == m_id )
    {
        setEnabled( enable );
    }
}

void LineEditID::setText2( QString text, int id )
{
    if ( id == m_id )
    {
        setText( text );
    }
}

void LineEditID::slotTextEdited( QString text )
{
    emit( textEdited2( text, m_id ) );
}
