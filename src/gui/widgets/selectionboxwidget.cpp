/*
 * selectionboxwidget.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */
#include <QtCore/QDebug>
#include <QtGui/QtGui>

#include "selectionboxwidget.h"

SelectionBoxWidget::SelectionBoxWidget( QWidget* parent ) :
    QFrame( parent ),
    m_count( 0 )
{
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setColumnCount( 1 );
    m_treeWidget->headerItem()->setText( 0, tr( "Selection Boxes" ) );
    m_treeWidget->header()->close();
    m_treeWidget->setRootIsDecorated( false );
    connect( m_treeWidget, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeClicked() ) );

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
    vLayout->addWidget( m_treeWidget );
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
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText( 0, QString( "new box " ) + QString::number( ++m_count ) );
    item->setCheckState( 0, Qt::CheckState::Checked );

    QList<QTreeWidgetItem*> sel = m_treeWidget->selectedItems();
    if( sel.size() > 0 )
    {
        if ( sel[0]->parent() )
        {
            sel[0]->parent()->addChild( item );
            sel[0]->parent()->setExpanded( true );
        }
        else
        {
            sel[0]->addChild( item );
            sel[0]->setExpanded( true );
        }
    }
    else
    {
        m_treeWidget->addTopLevelItem( item );
    }
}

void SelectionBoxWidget::treeClicked()
{
    qDebug() << "tree clicked";
    //m_treeWidget->clearSelection();
}
