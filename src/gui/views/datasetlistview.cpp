/*
 * datasetlistview.cpp
 *
 *  Created on: 08.05.2012
 *      Author: Ralph Schurade
 */
#include "datasetlistview.h"

#include <QtCore/QDebug>

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

void DatasetListView::rowsInserted (const QModelIndex &parent, int start, int end )
{
    selectionModel()->clear();
    selectionModel()->select( model()->index( start, 0, parent ), QItemSelectionModel::Select );
}
