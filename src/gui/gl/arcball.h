/*
 * arcball.h
 *
 * Created on: 10.05.2012
 * @author Ralph Schurade
 */

#ifndef ARCBALL_H_
#define ARCBALL_H_

#include "camerabase.h"

class ArcBall : public CameraBase
{
public:
    ArcBall( int width, int height );
    virtual ~ArcBall();

    void click( int x, int y );
    void drag( int x, int y );

    void midClick( int x, int y );
    void midDrag( int x, int y );

    void mouseWheel( float step );

    void setRotCenter( float x, float y, float z );

    QMatrix4x4 getMVMat();

    void setView( Fn::Orient view );

    float getMoveX();
    float getMoveY();

    QList<QVariant>getState();
    void setState( QList<QVariant> state );

    QQuaternion getRotation();
    void setRotation( QQuaternion quat );

private:
    QVector3D map_sphere( int x, int y );
    QQuaternion mat2quat( QMatrix4x4 &mat );

    float Epsilon;

    QVector3D v_mouse_current;      // mouse position at the beginning of dragging
    QVector3D v_mouse_down;         // mouse position at the beginning of dragging
    QVector3D v_from;

    QQuaternion q_current_rotation; // current rotation

    QMatrix4x4 m_currentRot;               // current rotation matrix
    QMatrix4x4 m_lastRot;               // current rotation matrix

    float m_moveX;
    float m_moveY;
    int m_oldMoveX;
    int m_oldMoveY;
    int m_midClickX;
    int m_midClickY;

    QVector3D m_rotCenter;
};

#endif /* ARCBALL_H_ */
