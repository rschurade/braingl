/*
 * checkboxwithlabel.cpp
 *
 *  Created on: 14.07.2012
 *      Author: Ralph Schurade
 */
#include "checkboxwithlabel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>

CheckboxWithLabel::CheckboxWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    m_checkBox = new QCheckBox( this );
    m_label = new QLabel( label, this );


    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget( m_label );
    hLayout->addStretch();
    hLayout->addWidget( m_checkBox );
    vLayout->addLayout( hLayout );

    hLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( vLayout );

    connect( m_checkBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotStateChanged( int ) ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px }" );
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

bool CheckboxWithLabel::checked()
{
    return m_checkBox->checkState();
}
