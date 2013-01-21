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

    m_lineEdit = new QLineEdit();

    QHBoxLayout* hLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();

    QGroupBox* gb = new QGroupBox( label );

    hLayout->addWidget( m_lineEdit );

    gb->setLayout( hLayout );
    gb->setFlat( true );
    vLayout->addWidget( gb );

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
