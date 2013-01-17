/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

#include "checkboxwithlabel.h"

CheckboxWithLabel::CheckboxWithLabel( QString label, int id )
{
    m_id = id;

    m_checkBox = new QCheckBox();

    QHBoxLayout* hLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();

    QGroupBox* gb = new QGroupBox( label );

    hLayout->addWidget( m_checkBox );

    gb->setLayout( hLayout );
    gb->setFlat( true );
    vLayout->addWidget( gb );

    setLayout( vLayout );

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
