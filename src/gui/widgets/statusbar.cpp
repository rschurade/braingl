/*
 * statusbar.cpp
 *
 *  Created on: Nov 13, 2012
 *      Author: Ralph Schurade
 */
#include "statusbar.h"

#include "../views/statusbarview.h"

#include "../../data/models.h"

StatusBar::StatusBar( QWidget* parent ) :
    QStatusBar( parent )
{
    m_statusBarView = new StatusBarView();
    m_statusBarView->setModel( Models::d() );

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
