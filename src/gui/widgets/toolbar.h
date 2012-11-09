/*
 * toolbar.h
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */

#ifndef TOOLBAR_H_
#define TOOLBAR_H_

#include <QtGui/QtGui>

class ToolBarView;

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    ToolBar( const QString &title, QWidget* parent=0 );
    virtual ~ToolBar();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    ToolBarView* m_toolBarView;

    QAction* m_scalarAction1;
    QAction* m_vectorAction1;
    QAction* m_multiAction1;
    QAction* m_multiAction1_2;
    QAction* m_multiAction1_3;
    QAction* m_multiAction2;
    QAction* m_multiAction3;
    QAction* m_multiAction4;
    QAction* m_meshAction1;
    QAction* m_fiberAction1;

    void createActions();

private slots:
    void slotScalar1();
    void slotVector1();
    void slotMulti1();
    void slotMulti1_2();
    void slotMulti1_3();
    void slotMulti2();
    void slotMulti3();
    void slotMulti4();

    void slotMesh1();
    void slotFiber1();

    void slotSelectionChanged( int type );
};

#endif /* TOOLBAR_H_ */
