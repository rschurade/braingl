#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "../../thirdparty/glew/include/glew.h"

#include "../gl/scenerenderer.h"

#include "../../data/enums.h"

#include <QtOpenGL/QGLWidget>

class SceneRenderer;
class QItemSelectionModel;

class GLWidget: public QGLWidget
{
    Q_OBJECT

public:
    GLWidget( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QItemSelectionModel* roiSelectionModel, QWidget *parent = 0 );
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setView( Fn::Orient view );

private:
    SceneRenderer* m_sceneRenderer;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void wheelEvent( QWheelEvent *event );

public slots:
    void update();

signals:

};

#endif
