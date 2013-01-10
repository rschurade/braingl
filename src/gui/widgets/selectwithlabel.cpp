/*
 * selectwithlabel.cpp
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */
#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>

#include "selectwithlabel.h"

SelectWithLabel::SelectWithLabel( QString label, int id )
{
    m_id = id;

    QHBoxLayout* layout = new QHBoxLayout();
    m_comboBox = new QComboBox();

    layout->addWidget( new QLabel( label ), 25 );
    layout->addWidget( m_comboBox, 75 );

    setLayout( layout );

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
