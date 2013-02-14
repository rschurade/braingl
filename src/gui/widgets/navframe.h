/*
 * navframe.h
 *
 *  Created on: Jan 23, 2013
 *      Author: Ralph Schurade
 */

#ifndef NAVFRAME_H_
#define NAVFRAME_H_

#include <QtCore/QString>
#include <QtGui/QFrame>

class QAbstractItemModel;
class NavGLWidget;
class QGLWidget;

class NavFrame : public QFrame
{
public:
    NavFrame( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QString name, int orient, QWidget *parent, const QGLWidget *shareWidget );
    virtual ~NavFrame();

    void update();

private:
    NavGLWidget* m_widget;
};

#endif /* NAVFRAME_H_ */
