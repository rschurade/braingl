/*
 * globalpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include "../views/globalpropertyview.h"

#include "globalpropertywidget.h"

GlobalPropertyWidget::GlobalPropertyWidget( QString name, QWidget* parent ) :
    QDockWidget( name, parent )
{
    setObjectName( name );

    m_propertyView = new GlobalPropertyView( this );
    setWidget( m_propertyView->getWidget() );
}

GlobalPropertyWidget::~GlobalPropertyWidget()
{
    // TODO Auto-generated destructor stub
}

void GlobalPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );
    m_propertyView->init();
}
