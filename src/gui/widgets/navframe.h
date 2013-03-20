/*
 * navframe.h
 *
 *  Created on: Jan 23, 2013
 *      Author: Ralph Schurade
 */

#ifndef NAVFRAME_H_
#define NAVFRAME_H_

#include <QString>
#include <QFrame>

class NavGLWidget;
class QGLWidget;

class NavFrame : public QFrame
{
public:
    NavFrame( QString name, int orient, QWidget *parent, const QGLWidget *shareWidget );
    virtual ~NavFrame();

    void update();
    void setWidgetVisible( bool visible );

private:
    NavGLWidget* m_widget;
};

#endif /* NAVFRAME_H_ */
