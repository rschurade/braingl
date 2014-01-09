/*
 * datasetlistview.cpp
 *
 * Created on: 08.05.2012
 * @author Ralph Schurade
 */
#include "datasetlistview.h"

#include <QDebug>

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

QModelIndex DatasetListView::getSelectedIndex( int column )
{

    if ( this->selectedIndexes().size() > 0 )
    {
        int sel = -1;
        sel = this->selectedIndexes().first().row();
        QModelIndex index = model()->index( sel, column );
        return index;
    }
    return QModelIndex();
}
