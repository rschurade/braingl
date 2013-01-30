/*
 * dockselectionboxwidget.h
 *
 *  Created on: Jan 30, 2013
 *      Author: schurade
 */

#ifndef DOCKSELECTIONBOXWIDGET_H_
#define DOCKSELECTIONBOXWIDGET_H_

#include "fndockwidget.h"

class SelectionBoxWidget;

class DockSelectionBoxWidget : public FNDockWidget
{
    Q_OBJECT

public:
    DockSelectionBoxWidget( QString name, QWidget* parent = 0 );
    virtual ~DockSelectionBoxWidget();

private:
    SelectionBoxWidget* m_widget;

public slots:
    void addBox();
};

#endif /* DOCKSELECTIONBOXWIDGET_H_ */
