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
class QVBoxLayout;
class QPushButton;

class SelectionBoxTreeView;

class SelectionBoxWidget : public QFrame
{
    Q_OBJECT

public:
    SelectionBoxWidget( QAbstractItemModel* selBoxModel, QWidget* parent = 0 );
    virtual ~SelectionBoxWidget();

private:
    SelectionBoxTreeView* m_treeView;

    QPushButton* m_newTopButton;
    QPushButton* m_newChildButton;
    QPushButton* m_deleteButton;

    int m_count;

public slots:
    void addBox();
    void treeClicked();
};

#endif /* SELECTIONBOXWIDGET_H_ */
