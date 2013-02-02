/*
 * selectionboxwidget.h
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */

#ifndef SELECTIONBOXWIDGET_H_
#define SELECTIONBOXWIDGET_H_

#include <QtGui/QFrame>

class QAbstractItemModel;
class QTreeView;
class QVBoxLayout;
class QPushButton;

class SelectionBoxWidget : public QFrame
{
    Q_OBJECT

public:
    SelectionBoxWidget( QAbstractItemModel* selBoxModel, QWidget* parent = 0 );
    virtual ~SelectionBoxWidget();

private:
    QTreeView* m_treeView;

    QPushButton* m_downButton;
    QPushButton* m_upButton;
    QPushButton* m_deleteButton;

    int m_count;

public slots:
    void addBox();
    void treeClicked();
};

#endif /* SELECTIONBOXWIDGET_H_ */
