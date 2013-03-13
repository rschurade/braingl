/*
 * pathwidgetwithlabel.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: schurade
 */

#include "pathwidgetwithlabel.h"

#include <QDebug>
#include <QVariant>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

PathWidgetWithLabel::PathWidgetWithLabel( QString label, int id, QWidget* parent )  :
    QFrame( parent ),
    m_id( id ),
    m_value( QDir( "" ) )
{
    m_button = new QPushButton( QString( "set dir" ), this );
    m_label = new QLabel( label, this );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    if ( label != "" )
    {
        hLayout->addWidget( m_label );
        hLayout->addStretch();
    }
    hLayout->addWidget( m_button );
    vLayout->addLayout( hLayout );

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    m_edit = new QLineEdit( this );
    m_edit->setReadOnly( true );
    hLayout2->addWidget( m_edit );
    vLayout->addLayout( hLayout2 );

    hLayout->setContentsMargins( 1, 1, 1, 1 );
    hLayout2->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( vLayout );

    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotButton() ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
}

PathWidgetWithLabel::~PathWidgetWithLabel()
{
}


void PathWidgetWithLabel::setValue( QDir value )
{
    m_value = value;
    m_edit->setText( m_value.absolutePath() );
}

void PathWidgetWithLabel::slotButton()
{
    m_value = QFileDialog::getExistingDirectory();
    m_edit->setText( m_value.absolutePath() );
    emit( valueChanged( m_value, m_id ) );
}

