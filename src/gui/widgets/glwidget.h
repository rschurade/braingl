#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "../gl/scenerenderer.h"

#include <QtOpenGL/QGLWidget>

class DataStore;
class SceneRenderer;

class GLWidget: public QGLWidget
{
    Q_OBJECT

public:
    GLWidget( DataStore* dataStore, QWidget *parent = 0 );
    ~GLWidget();

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

private:
    DataStore* m_dataStore;
    SceneRenderer* m_sceneRenderer;
};

#endif
