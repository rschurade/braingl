/*
 * datasetviewwidget.h
 *
 * Created on: 08.05.2012
 * @author Ralph Schurade
 */

#ifndef DATASETVIEWWIDGET_H_
#define DATASETVIEWWIDGET_H_

#include <QWidget>

class QVBoxLayout;
class QPushButton;
class DatasetListView;
class QItemSelection;
class QItemSelectionModel;

class DatasetListWidget : public QWidget
{
	Q_OBJECT

public:
	DatasetListWidget( QWidget* parent = 0 );
	virtual ~DatasetListWidget();

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
