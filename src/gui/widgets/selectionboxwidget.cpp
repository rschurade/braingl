/*
 * selectionboxwidget.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */
#include "selectionboxwidget.h"

#include <QtCore/QDebug>
#include <QtGui/QtGui>

SelectionBoxWidget::SelectionBoxWidget( QAbstractItemModel* selBoxModel, QWidget* parent ) :
    QFrame( parent ),
    m_count( 0 )
{
    m_treeView = new QTreeView();
    m_treeView->setModel( selBoxModel );
    connect( m_treeView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeClicked() ) );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_downButton = new QPushButton( this );
    m_downButton->setText( QString( "down" ) );
    m_upButton = new QPushButton( this );
    m_upButton->setText( QString( "up" ) );
    m_deleteButton = new QPushButton( this );
    m_deleteButton->setText( QString( "delete" ) );

    buttonLayout->addWidget( m_downButton );
    buttonLayout->addWidget( m_upButton );
    buttonLayout->addWidget( m_deleteButton );

//    connect( m_upButton, SIGNAL( clicked() ), this, SLOT( moveItemUp() ) );
//    connect( m_downButton, SIGNAL( clicked() ), this, SLOT( moveItemDown() ) );
//    connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteItem() ) );

    buttonLayout->setContentsMargins( 0, 0, 0, 0 );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget( m_treeView );
    vLayout->addLayout( buttonLayout );
    vLayout->setContentsMargins( 1, 1, 1, 1 );

    vLayout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( vLayout );
    setFrameStyle( QFrame::StyledPanel | QFrame::Plain );


    m_upButton->setEnabled( false );
    m_downButton->setEnabled( false );
    m_deleteButton->setEnabled( false );

    setStyleSheet( "QPushButton { font:  bold 12px; max-height: 14px; margin-top: -1px } " );

}

SelectionBoxWidget::~SelectionBoxWidget()
{
}

void SelectionBoxWidget::addBox()
{

}

void SelectionBoxWidget::treeClicked()
{
    qDebug() << "tree clicked";
    //m_treeWidget->clearSelection();
}
