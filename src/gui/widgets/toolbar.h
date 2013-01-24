/*
 * toolbar.h
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */

#ifndef TOOLBAR_H_
#define TOOLBAR_H_

#include <QtGui/QtGui>

#include "../../data/enums.h"

#include "fnaction.h"

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

    FNAction* m_isosurfaceAct;
    FNAction* m_testAct;
    FNAction* m_vectorAction1;
    FNAction* m_qball4Act;
    FNAction* m_qball6Act;
    FNAction* m_qball8Act;
    FNAction* m_tensorAct;
    FNAction* m_faAct;
    FNAction* m_evAct;
    FNAction* m_meshAction1;
    FNAction* m_fiberTrackingAct;
    FNAction* m_fiberThinningAct;
    FNAction* m_binghamAction;
    FNAction* m_bingham2tensorAction;

    void createActions();

private slots:
    void slot( FN_ALGO algo );
    void slotSelectionChanged( int type );
};

#endif /* TOOLBAR_H_ */
