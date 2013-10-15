/*
 * matrixwidget.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#include "matrixwidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

MatrixWidget::MatrixWidget( QString name, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_digits( 2 )
{
    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout0 = new QHBoxLayout();
    m_00 = makeEdit();
    m_01 = makeEdit();
    m_02 = makeEdit();
    m_03 = makeEdit();
    hLayout0->addWidget( m_00 );
    hLayout0->addWidget( m_01 );
    hLayout0->addWidget( m_02 );
    hLayout0->addWidget( m_03 );

    m_10 = makeEdit();
    m_11 = makeEdit();
    m_12 = makeEdit();
    m_13 = makeEdit();
    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget( m_10 );
    hLayout1->addWidget( m_11 );
    hLayout1->addWidget( m_12 );
    hLayout1->addWidget( m_13 );

    m_20 = makeEdit();
    m_21 = makeEdit();
    m_22 = makeEdit();
    m_23 = makeEdit();
    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_20 );
    hLayout2->addWidget( m_21 );
    hLayout2->addWidget( m_22 );
    hLayout2->addWidget( m_23 );

    m_30 = makeEdit();
    m_31 = makeEdit();
    m_32 = makeEdit();
    m_33 = makeEdit();
    QHBoxLayout* hLayout3 = new QHBoxLayout();
    hLayout3->addWidget( m_30 );
    hLayout3->addWidget( m_31 );
    hLayout3->addWidget( m_32 );
    hLayout3->addWidget( m_33 );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout0 );
    vLayout->addLayout( hLayout1 );
    vLayout->addLayout( hLayout2 );
    vLayout->addLayout( hLayout3 );

    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }" );
}

MatrixWidget::~MatrixWidget()
{
}

QLineEdit* MatrixWidget::makeEdit()
{
    QLineEdit* out = new QLineEdit();
    out->setMaxLength( 10 );
    out->setMaximumWidth( 100 );
    out->setAlignment( Qt::AlignCenter );
    connect( out, SIGNAL( textEdited( QString ) ), this, SLOT( editChanged() ) );
    return out;
}

void MatrixWidget::setValue( QMatrix4x4 value )
{
    m_00->setText( QString::number( value( 0, 0 ) ) );
    m_01->setText( QString::number( value( 0, 1 ) ) );
    m_02->setText( QString::number( value( 0, 2 ) ) );
    m_03->setText( QString::number( value( 0, 3 ) ) );

    m_10->setText( QString::number( value( 1, 0 ) ) );
    m_11->setText( QString::number( value( 1, 1 ) ) );
    m_12->setText( QString::number( value( 1, 2 ) ) );
    m_13->setText( QString::number( value( 1, 3 ) ) );

    m_20->setText( QString::number( value( 2, 0 ) ) );
    m_21->setText( QString::number( value( 2, 1 ) ) );
    m_22->setText( QString::number( value( 2, 2 ) ) );
    m_23->setText( QString::number( value( 2, 3 ) ) );

    m_30->setText( QString::number( value( 3, 0 ) ) );
    m_31->setText( QString::number( value( 3, 1 ) ) );
    m_32->setText( QString::number( value( 3, 2 ) ) );
    m_33->setText( QString::number( value( 3, 3 ) ) );
}

void MatrixWidget::editChanged()
{
    QMatrix4x4 m;
    m( 0, 0 ) = m_00->text().toFloat();
    m( 0, 1 ) = m_01->text().toFloat();
    m( 0, 2 ) = m_02->text().toFloat();
    m( 0, 3 ) = m_03->text().toFloat();

    m( 1, 0 ) = m_10->text().toFloat();
    m( 1, 1 ) = m_11->text().toFloat();
    m( 1, 2 ) = m_12->text().toFloat();
    m( 1, 3 ) = m_13->text().toFloat();

    m( 2, 0 ) = m_20->text().toFloat();
    m( 2, 1 ) = m_21->text().toFloat();
    m( 2, 2 ) = m_22->text().toFloat();
    m( 2, 3 ) = m_23->text().toFloat();

    m( 3, 0 ) = m_30->text().toFloat();
    m( 3, 1 ) = m_31->text().toFloat();
    m( 3, 2 ) = m_32->text().toFloat();
    m( 3, 3 ) = m_33->text().toFloat();

    emit( valueChanged( m_id,m ) );
}
