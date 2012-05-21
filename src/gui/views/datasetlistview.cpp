/*
 * datasetlistview.cpp
 *
 *  Created on: 08.05.2012
 *      Author: Ralph
 */

#include "datasetlistview.h"

DatasetListView::DatasetListView( QWidget *parent ) : QListView( parent )
{
}

DatasetListView::~DatasetListView()
{
}

void DatasetListView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
	emit itemSelectionChanged( selected );
}
