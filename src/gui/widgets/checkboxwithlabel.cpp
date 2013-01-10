/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

#include "checkboxwithlabel.h"

CheckboxWithLabel::CheckboxWithLabel( QString label, int id )
{
    m_id = id;

    QHBoxLayout* layout = new QHBoxLayout();
    m_checkBox = new QCheckBox();

    layout->addWidget( new QLabel( label ), 25 );
    layout->addWidget( m_checkBox, 75 );

    setLayout( layout );

    connect( m_checkBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotStateChanged( int ) ) );
}


CheckboxWithLabel::~CheckboxWithLabel()
{
}

void CheckboxWithLabel::slotStateChanged( int state )
{
    emit( stateChanged( state, m_id ) );
}

void CheckboxWithLabel::setChecked( bool state )
{
    m_checkBox->setChecked( state );
}
