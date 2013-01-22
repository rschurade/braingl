/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

#include "checkboxwithlabel.h"

CheckboxWithLabel::CheckboxWithLabel( QString label, int id )
{
    m_id = id;

    m_checkBox = new QCheckBox();
    m_label = new QLabel( label );


    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget( m_label );
    hLayout->addStretch();
    hLayout->addWidget( m_checkBox );
    vLayout->addLayout( hLayout );

    hLayout->setContentsMargins( 0,0,0,0 );
    vLayout->setContentsMargins( 0,0,0,0 );

//    QHBoxLayout* hLayout2 = new QHBoxLayout();
//
//    hLayout2->addStretch();
//    vLayout->addLayout( hLayout2 );

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
