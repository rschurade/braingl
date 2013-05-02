/*
 * glwidget.h
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "GL/glew.h"

#include "../gl/scenerenderer.h"

#include "../../data/enums.h"

#include <QtOpenGL/QGLWidget>

class SceneRenderer;
class QItemSelectionModel;

class GLWidget: public QGLWidget
{
    Q_OBJECT

public:
    GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent = 0 );
    GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent, const QGLWidget *shareWidget );
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setView( Fn::Orient view );
    QImage* screenshot();

private:
    SceneRenderer* m_sceneRenderer;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void enterEvent( QEvent *event );
    void wheelEvent( QWheelEvent *event );

public slots:
    void update();

signals:

};

#endif
