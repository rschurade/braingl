/*
 * selectionboxtreeview.h
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#ifndef SELECTIONBOXTREEVIEW_H_
#define SELECTIONBOXTREEVIEW_H_

#include <QtGui/QMouseEvent>
#include <QtGui/QTreeView>

class SelectionBoxTreeView : public QTreeView
{
public:
    SelectionBoxTreeView( QWidget* parent = 0 );
    virtual ~SelectionBoxTreeView();

    void addBox();

private:
    virtual void mousePressEvent(QMouseEvent *event);

};

#endif /* SELECTIONBOXTREEVIEW_H_ */
