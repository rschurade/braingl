/*
 * datasetviewwidget.h
 *
 *  Created on: 08.05.2012
 *      Author: Ralph
 */

#ifndef DATASETVIEWWIDGET_H_
#define DATASETVIEWWIDGET_H_

#include "fndockwidget.h"

class QVBoxLayout;
class QPushButton;
class QAbstractItemModel;
class DatasetListView;
class QItemSelection;
class QItemSelectionModel;

class DatasetListWidget : public FNDockWidget
{
	Q_OBJECT

public:
	DatasetListWidget( QWidget* parent = 0 );
	virtual ~DatasetListWidget();

	void setModel ( QAbstractItemModel  *model );
	QItemSelectionModel* selectionModel();

	int getSelected();

private:
	DatasetListView* m_listView;
	QVBoxLayout* m_layout;

	QPushButton* m_downButton;
	QPushButton* m_upButton;
	QPushButton* m_deleteButton;

	int m_selected;

private slots:
	void moveItemUp();
	void moveItemDown();
	void deleteItem();
	void itemSelectionChanged( const QItemSelection &selected );

signals:
	void moveSelectedItemUp( int row );
	void moveSelectedItemDown( int row );
	void deleteSelectedItem( int row );
};

#endif /* DATASETVIEWWIDGET_H_ */
