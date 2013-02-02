#ifndef COMBINEDNAVGLWIDGET_H
#define COMBINEDNAVGLWIDGET_H

#include "../../thirdparty/glew/include/glew.h"
#include <QtOpenGL/QGLWidget>

class CombinedNavRenderer;
class QAbstractItemModel;

class CombinedNavGLWidget: public QGLWidget
{
    Q_OBJECT

public:
    CombinedNavGLWidget( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QString name, QWidget *parent = 0, const QGLWidget *shareWidget = 0 );
    ~CombinedNavGLWidget();

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
    CombinedNavRenderer* m_renderer;
};

#endif
