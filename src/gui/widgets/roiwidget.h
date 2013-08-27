/*
 * roiwidget.h
 *
 * Created on: Jan 30, 2013
 * @author Ralph Schurade
 */

#ifndef ROIWIDGET_H_
#define ROIWIDGET_H_

#include <QFrame>

class QAbstractItemModel;
class QVBoxLayout;
class QPushButton;
class QItemSelection;
class QItemSelectionModel;
class ROITreeView;
class ROIArea;

class ROIWidget : public QFrame
{
    Q_OBJECT

public:
    ROIWidget( QWidget* parent = 0 );
    virtual ~ROIWidget();

    QItemSelectionModel* selectionModel();

    void keyPressEvent( QKeyEvent* event );

private:
    ROITreeView* m_treeView;

    QPushButton* m_newTopButton;
    QPushButton* m_newChildButton;
    QPushButton* m_deleteButton;

    int m_count;

public slots:
    void addBox();
    void addROIArea( ROIArea* roi );
    void deleteItem();
    void treeClicked();
    void itemSelectionChanged( const QItemSelection &selected );
};

#endif /* ROIWIDGET_H_ */
