/*
 * datasetpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include "../views/datasetpropertyview.h"

#include "datasetpropertywidget.h"

DatasetPropertyWidget::DatasetPropertyWidget( QString name, QWidget* parent ) :
    QDockWidget( name, parent )
{
    setObjectName( name );

    m_propertyView = new DatasetPropertyView( this );
    setWidget( m_propertyView->getWidget() );
}

DatasetPropertyWidget::~DatasetPropertyWidget()
{
}

void DatasetPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );

}

void DatasetPropertyWidget::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_propertyView->setSelectionModel( selectionModel );
}
