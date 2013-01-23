/*
 * navframe.h
 *
 *  Created on: Jan 23, 2013
 *      Author: schurade
 */

#ifndef NAVFRAME_H_
#define NAVFRAME_H_

#include <QtCore/QString>
#include <QtGui/QFrame>

class DataStore;
class NavGLWidget;
class QGLWidget;

class NavFrame : public QFrame
{
public:
    NavFrame( DataStore* dataStore, QString name, int orient, QWidget *parent, const QGLWidget *shareWidget );
    virtual ~NavFrame();

private:
    NavGLWidget* m_widget;
};

#endif /* NAVFRAME_H_ */
