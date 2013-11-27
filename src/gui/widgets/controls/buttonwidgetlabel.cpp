/*
 * pathwidgetwithlabel.cpp
 *
 * Created on: Mar 6, 2013
 * @author schurade
 */

#include "buttonwithlabel.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ButtonWithLabel::ButtonWithLabel( QString label, int id, QWidget* parent )  :
    QFrame( parent ),
    m_id( id )
{
    m_button = new QPushButton( QString( "press" ), this );
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

    hLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( vLayout );

    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotButton() ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
}


ButtonWithLabel::~ButtonWithLabel()
{
}

void ButtonWithLabel::slotButton()
{
    emit( buttonPressed( m_id ) );
}

