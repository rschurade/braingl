/*
 * datasetlistview.h
 *
 *  Created on: 08.05.2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETLISTVIEW_H_
#define DATASETLISTVIEW_H_

#include <QListView>

class DatasetListView : public QListView
{
	Q_OBJECT

public:
	DatasetListView( QWidget  *parent=0 );
	virtual ~DatasetListView();

private slots:
	void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );
	void rowsInserted (const QModelIndex &parent, int start, int end );

signals:
	void itemSelectionChanged( const QItemSelection &selected );
};

#endif /* DATASETLISTVIEW_H_ */
