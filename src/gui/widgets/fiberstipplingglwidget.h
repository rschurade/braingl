/*
 * fiberstipplingglwidget.h
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#ifndef FIBERSTIPPLINGGLWIDGET_H_
#define FIBERSTIPPLINGGLWIDGET_H_

#include "GL/glew.h"

#include <QtOpenGL/QGLWidget>

class FiberStipplingRenderer;

class FiberStipplingGLWidget: public QGLWidget
{
    Q_OBJECT

public:
    FiberStipplingGLWidget( QString name, QWidget *parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~FiberStipplingGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    FiberStipplingRenderer* renderer() { return m_renderer; };

public slots:
    void update();
    void setWidgetVisible( bool visible );

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
    FiberStipplingRenderer* m_renderer;
    bool m_visible;
};

#endif /* FIBERSTIPPLINGGLWIDGET_H_ */
