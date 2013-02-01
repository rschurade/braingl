/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */
#include "editwithlabel.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>

EditWithLabel::EditWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( label, this );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    m_lineEdit = new QLineEdit( this );
    hLayout2->addWidget( m_lineEdit );
    hLayout2->addStretch();

    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout2 );
    vLayout->setContentsMargins( 1,1,1,1 );

    setLayout( vLayout );

    connect( m_lineEdit, SIGNAL( textEdited( QString ) ), this, SLOT( slotTextChanged( QString ) ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }"
                   );
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
