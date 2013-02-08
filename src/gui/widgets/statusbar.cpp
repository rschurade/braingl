/*
 * statusbar.cpp
 *
 *  Created on: Nov 13, 2012
 *      Author: Ralph Schurade
 */
#include "statusbar.h"

#include "../views/statusbarview.h"

StatusBar::StatusBar( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QWidget* parent ) :
    QStatusBar( parent )
{
    m_statusBarView = new StatusBarView( globalModel );
    m_statusBarView->setModel( dataModel );

    connect( m_statusBarView, SIGNAL( sigStatusChanged( QString ) ), this, SLOT( slotStatusChanged( QString ) ) );
}

StatusBar::~StatusBar()
{
}

void StatusBar::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_statusBarView->setSelectionModel( selectionModel );
}

void StatusBar::slotStatusChanged( QString text )
{
    this->showMessage( text );
}
