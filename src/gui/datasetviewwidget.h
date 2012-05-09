/*
 * datasetviewwidget.h
 *
 *  Created on: 08.05.2012
 *      Author: Ralph
 */

#ifndef DATASETVIEWWIDGET_H_
#define DATASETVIEWWIDGET_H_

#include <QtGui/QDockWidget>

class QVBoxLayout;
class QPushButton;
class QAbstractItemModel;
class DatasetListView;
class QItemSelection;
class QItemSelectionModel;

class DatasetViewWidget : public QDockWidget
{
	Q_OBJECT

public:
	DatasetViewWidget( QWidget* parent = 0 );
	virtual ~DatasetViewWidget();

	void setModel ( QAbstractItemModel  *model );
	QItemSelectionModel* selectionModel();

private:
	DatasetListView* m_listView;
	QVBoxLayout* m_layout;

	QPushButton* m_downButton;
	QPushButton* m_upButton;

	int m_selected;

private slots:
	void moveItemUp();
	void moveItemDown();
	void itemSelectionChanged( const QItemSelection &selected );

signals:
	void moveSelectedItemUp( int row );
	void moveSelectedItemDown( int row );
};

#endif /* DATASETVIEWWIDGET_H_ */
