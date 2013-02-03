/*
 * roiwidget.h
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */

#ifndef ROIWIDGET_H_
#define ROIWIDGET_H_

#include <QtGui/QFrame>

class QAbstractItemModel;
class QVBoxLayout;
class QPushButton;
class QItemSelectionModel;
class ROITreeView;

class ROIWidget : public QFrame
{
    Q_OBJECT

public:
    ROIWidget( QAbstractItemModel* ROIModel, QWidget* parent = 0 );
    virtual ~ROIWidget();

    QItemSelectionModel* selectionModel();

private:
    ROITreeView* m_treeView;

    QPushButton* m_newTopButton;
    QPushButton* m_newChildButton;
    QPushButton* m_deleteButton;

    int m_count;

public slots:
    void addBox();
    void treeClicked();
};

#endif /* ROIWIDGET_H_ */
