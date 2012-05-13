/*
 * datasetinfowidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#include "datasetinfoview.h"
#include "datasetinfowidget.h"

DatasetInfoWidget::DatasetInfoWidget( QString name, QWidget* parent ) :
QDockWidget( name, parent )
{
    setObjectName( name );

    m_infoView = new DatasetInfoView( this );
    m_infoView->hideColumn( 0 );
    setWidget( m_infoView );
}

DatasetInfoWidget::~DatasetInfoWidget()
{
}

void DatasetInfoWidget::setModel( QAbstractItemModel* model )
{
    m_infoView->setModel( model );

}

void DatasetInfoWidget::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_infoView->setSelectionModel( selectionModel );
}
