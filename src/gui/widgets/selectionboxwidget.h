/*
 * selectionboxwidget.h
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */

#ifndef SELECTIONBOXWIDGET_H_
#define SELECTIONBOXWIDGET_H_

#include <QtGui/QFrame>

class QTreeWidget;
class QVBoxLayout;
class QPushButton;

class SelectionBoxWidget : public QFrame
{
    Q_OBJECT

public:
    SelectionBoxWidget( QWidget* parent );
    virtual ~SelectionBoxWidget();

private:
    QTreeWidget* m_treeWidget;

    QPushButton* m_downButton;
    QPushButton* m_upButton;
    QPushButton* m_deleteButton;

    int m_count;

public slots:
    void addBox();
    void treeClicked();
};

#endif /* SELECTIONBOXWIDGET_H_ */
