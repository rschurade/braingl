/*
 * roitreeview.h
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#ifndef ROITREEVIEW_H_
#define ROITREEVIEW_H_

#include <QtGui/QMouseEvent>
#include <QtGui/QTreeView>

class ROITreeView : public QTreeView
{
public:
    ROITreeView( QWidget* parent = 0 );
    virtual ~ROITreeView();

    void addBox();

private:
    virtual void mousePressEvent( QMouseEvent *event );

};

#endif /* ROITREEVIEW_H_ */
