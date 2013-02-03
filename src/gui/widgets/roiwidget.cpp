/*
 * roiwidget.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */
#include "roiwidget.h"

#include "../views/roitreeview.h"

#include <QtCore/QDebug>
#include <QtGui/QtGui>

ROIWidget::ROIWidget( QAbstractItemModel* roiModel, QWidget* parent ) :
    QFrame( parent ),
    m_count( 0 )
{
    m_treeView = new ROITreeView( this );
    m_treeView->setModel( roiModel );
    m_treeView->hideColumn( 1 );
    connect( m_treeView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeClicked() ) );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_newTopButton = new QPushButton( this );
    m_newTopButton->setText( QString( "new top" ) );
    m_newChildButton = new QPushButton( this );
    m_newChildButton->setText( QString( "new child" ) );
    m_deleteButton = new QPushButton( this );
    m_deleteButton->setText( QString( "delete" ) );

    buttonLayout->addWidget( m_newTopButton );
    buttonLayout->addWidget( m_newChildButton );
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


    m_newTopButton->setEnabled( false );
    m_newChildButton->setEnabled( false );
    m_deleteButton->setEnabled( false );

    setStyleSheet( "QPushButton { font:  bold 12px; max-height: 14px; margin-top: -1px } " );

}

ROIWidget::~ROIWidget()
{
}

void ROIWidget::addBox()
{
    m_treeView->addBox();
}

void ROIWidget::treeClicked()
{
}

QItemSelectionModel* ROIWidget::selectionModel()
{
    return m_treeView->selectionModel();
}
