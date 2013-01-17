/*
 * selectwithlabel.cpp
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>

#include "selectwithlabel.h"

SelectWithLabel::SelectWithLabel( QString label, int id )
{
    m_id = id;

    m_comboBox = new QComboBox();

    QHBoxLayout* hLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();

    QGroupBox* gb = new QGroupBox( label );
    hLayout->addWidget( m_comboBox );

    gb->setLayout( hLayout );
    gb->setFlat( true );
    vLayout->addWidget( gb );

    setLayout( vLayout );

    connect( m_comboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( indexChanged( int ) ) );
}

SelectWithLabel::~SelectWithLabel()
{
}

void SelectWithLabel::insertItem( int index, QString label )
{
    m_comboBox->insertItem( index, label );
}

void SelectWithLabel::setCurrentIndex( int index )
{
    m_comboBox->setCurrentIndex( index );
}

void SelectWithLabel::indexChanged( int index )
{
    emit( currentIndexChanged( index, m_id ) );
}
