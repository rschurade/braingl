/*
 * datasetlistview.h
 *
 *  Created on: 08.05.2012
 *      Author: Ralph
 */

#ifndef DATASETLISTVIEW_H_
#define DATASETLISTVIEW_H_

#include <QtGui/QListView>

class DatasetListView : public QListView
{
	Q_OBJECT

public:
	DatasetListView( QWidget  *parent=0 );
	virtual ~DatasetListView();

private slots:
	void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );

signals:
	void itemSelectionChanged( const QItemSelection &selected );
};

#endif /* DATASETLISTVIEW_H_ */
