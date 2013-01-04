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

    QAction* m_isosurfaceAct;
    QAction* m_vectorAction1;
    QAction* m_qball4Act;
    QAction* m_qball6Act;
    QAction* m_qball8Act;
    QAction* m_tensorAct;
    QAction* m_faAct;
    QAction* m_evAct;
    QAction* m_meshAction1;
    QAction* m_fiberAction1;
    QAction* m_binghamAction;

    void createActions();

private slots:
    void slotIsosurface();
    void slotVector1();
    void slotQBall4();
    void slotQBall6();
    void slotQBall8();
    void slotTensor();
    void slotFA();
    void slotEV();
    void slotBingham();

    void slotMesh1();
    void slotFiber1();

    void slotSelectionChanged( int type );
};

#endif /* TOOLBAR_H_ */
