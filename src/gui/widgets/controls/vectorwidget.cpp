/*
 * vectorwidget.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: schurade
 */

#include "vectorwidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

VectorWidget::VectorWidget( QString name, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_digits( 4 )
{
    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout0 = new QHBoxLayout();
    m_00 = makeEdit();
    m_01 = makeEdit();
    m_02 = makeEdit();
    hLayout0->addWidget( m_00 );
    hLayout0->addWidget( m_01 );
    hLayout0->addWidget( m_02 );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout0 );

    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }" );
}

VectorWidget::~VectorWidget()
{
}

QLineEdit* VectorWidget::makeEdit()
{
    QLineEdit* out = new QLineEdit();
    out->setMaxLength( 10 );
    out->setMaximumWidth( 100 );
    out->setAlignment( Qt::AlignCenter );
    connect( out, SIGNAL( textEdited( QString ) ), this, SLOT( editChanged() ) );
    return out;
}

void VectorWidget::setValue( QVector3D value )
{
    m_00->setText( QString::number( value.x() ) );
    m_01->setText( QString::number( value.y() ) );
    m_02->setText( QString::number( value.z() ) );
}

void VectorWidget::editChanged()
{
    QVector3D v( m_00->text().toFloat(), m_01->text().toFloat(), m_02->text().toFloat() );

    emit( valueChanged( m_id, v ) );
}
