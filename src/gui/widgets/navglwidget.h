/*
 * navglwidget.h
 *
 *  Created on: May 03, 2012
 *      Author: Ralph Schurade
 */
#ifndef NAVGLWIDGET_H
#define NAVGLWIDGET_H

#include "GL/glew.h"

#include <QtOpenGL/QGLWidget>

class NavRenderer;
class QAbstractItemModel;

class NavGLWidget: public QGLWidget
{
    Q_OBJECT

public:
    NavGLWidget( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QString name, int orient, QWidget *parent = 0, const QGLWidget *shareWidget = 0 );
    ~NavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void update();

signals:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void wheelEvent( QWheelEvent *event );

private:
    NavRenderer* m_navRenderer;
};

#endif
