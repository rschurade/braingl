/*
 * glwidget.h
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "../gl/scenerenderer.h"

#include "../../data/enums.h"

#include <QGLWidget>

class ArcBall;
class Camera;
class CameraBase;
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
    void screenshot( QString fn, bool exitAfter = false );

    CameraBase* getCameraInUse();
    ArcBall* getArcBall();
    Camera* getCamera();

    void getCameraParametersFromModelviewMatrix( QVector3D &eyepos,  QVector3D &viewdir, QVector3D &updir );

private:
    QString m_name;

    QItemSelectionModel* m_roiSelectionModel;

    CameraBase* m_cameraInUse;
    ArcBall* m_arcBall;
    Camera* m_camera;

    SceneRenderer* m_sceneRenderer;

    QMatrix4x4 m_mvMatrix;
    QMatrix4x4 m_pMatrix;

    bool m_visible;

    int m_picked;
    QVector2D m_rightMouseDown;
    QVector2D m_pickOld;
    int m_sliceXPosAtPick;
    int m_sliceYPosAtPick;
    int m_sliceZPosAtPick;
    bool skipDraw;

    int m_width;
    int m_height;

    bool m_doScreenshot;
    bool m_exitAfterScreenshot;
    QString m_screenshotFileName;

    int m_copyCameraMode;

    void calcMVPMatrix();

    void rightMouseDown( QMouseEvent* event );
    void rightMouseDrag( QMouseEvent* event );

    void cameraCircle( bool dir );

    void showValuePickTooltip( QMouseEvent* event );

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
    void enterEvent( QEvent* event );
    void wheelEvent( QWheelEvent* event );
    void keyPressEvent( QKeyEvent* event );

public slots:
    void update();
    void visibilityChanged( bool visible );

signals:
    void signalKeyPressed( int key, Qt::KeyboardModifiers mods );
    void signalCameraChanged();
    void signalCopyCameraToScript( int mode );
};

#endif
