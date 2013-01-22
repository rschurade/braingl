/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>

#include "editwithlabel.h"

EditWithLabel::EditWithLabel( QString label, int id )
{
    m_id = id;

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( label );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    m_lineEdit = new QLineEdit();
    hLayout2->addWidget( m_lineEdit );
    hLayout2->addStretch();

    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout2 );
    vLayout->setContentsMargins( 0,0,0,0 );

    setLayout( vLayout );

    connect( m_lineEdit, SIGNAL( textEdited( QString ) ), this, SLOT( slotTextChanged( QString ) ) );
}


EditWithLabel::~EditWithLabel()
{
}

void EditWithLabel::slotTextChanged( QString text )
{
    emit( valueChanged( text, m_id ) );
}

void EditWithLabel::setText( QString text )
{
    m_lineEdit->setText( text );
}
