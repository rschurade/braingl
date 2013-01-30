/*
 * dockselectionboxwidget.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */
#include "../widgets/selectionboxwidget.h"

#include "dockselectionboxwidget.h"

DockSelectionBoxWidget::DockSelectionBoxWidget( QString name, QWidget* parent ) :
   FNDockWidget( name, parent )
{
    setObjectName( "Selection Box Dock" );

    m_widget = new SelectionBoxWidget( this );
    setWidget( m_widget );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );
}

DockSelectionBoxWidget::~DockSelectionBoxWidget()
{
}

void DockSelectionBoxWidget::addBox()
{
    m_widget->addBox();
}
