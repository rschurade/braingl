/*
 * statusbar.cpp
 *
 *  Created on: Nov 13, 2012
 *      Author: schurade
 */
#include "statusbar.h"

#include "../views/statusbarview.h"

StatusBar::StatusBar( QWidget* parent ) :
    QStatusBar( parent )
{
    m_statusBarView = new StatusBarView();

    connect( m_statusBarView, SIGNAL( sigStatusChanged( QString ) ), this, SLOT( slotStatusChanged( QString ) ) );
}

StatusBar::~StatusBar()
{
}

void StatusBar::setModel( QAbstractItemModel* model )
{
    m_statusBarView->setModel( model );
}

void StatusBar::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_statusBarView->setSelectionModel( selectionModel );
}

void StatusBar::slotStatusChanged( QString text )
{
    this->showMessage( text );
}
