/*
 * colorwidgetwithlabel.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: Ralph Schurade
 */
#include "colorwidgetwithlabel.h"

#include <QtGui/QColorDialog>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

ColorWidgetWithLabel::ColorWidgetWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_value( QColor( 0, 0, 0 ) )
{
    m_button = new QPushButton( QString( "set color" ), this );
    m_label = new QLabel( label, this );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget( m_label );
    hLayout->addStretch();
    hLayout->addWidget( m_button );
    vLayout->addLayout( hLayout );

    hLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( vLayout );

    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotButton() ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QFrame { background-color : " + m_value.name() + " }"
                     "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
}

ColorWidgetWithLabel::~ColorWidgetWithLabel()
{
}

void ColorWidgetWithLabel::setValue( QColor value )
{
    m_value = value;
    setStyleSheet( "QFrame { background-color : " + m_value.name() + " }"
                     "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
}

void ColorWidgetWithLabel::slotButton()
{
    m_value = QColorDialog::getColor( m_value );

    setStyleSheet( "QFrame { background-color : " + m_value.name() + " }"
                     "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
    emit( colorChanged( m_value, m_id ) );
}
