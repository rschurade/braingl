/*
 * arcball.h
 *
 *  Created on: 10.05.2012
 *      Author: Ralph
 */

#ifndef ARCBALL_H_
#define ARCBALL_H_

#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

class ArcBall
{
public:
    ArcBall( int width, int height );
    virtual ~ArcBall();

    void set_win_size( int width,  int height );

    void click( int x, int y );
    void drag( int x, int y );

    void midClick( int x, int y );
    void midDrag( int x, int y );

    void mouseWheel( int step );

    void setRotCenter( float x, float y, float z );

    QMatrix4x4 getMVMat();

    void setView( int view );

private:
    QVector3D map_sphere( int x, int y );

    float Epsilon;
    int m_width;  // width of window
    int m_height; // height of window
    float m_adjust_width;
    float m_adjust_height;

    QVector3D v_mouse_current;      // mouse position at the beginning of dragging
    QVector3D v_mouse_down;         // mouse position at the beginning of dragging
    QVector3D v_from;

    QQuaternion q_current_rotation; // current rotation

    QMatrix4x4 m_currentRot;               // current rotation matrix
    QMatrix4x4 m_lastRot;               // current rotation matrix

    int m_moveX;
    int m_moveY;
    int m_oldMoveX;
    int m_oldMoveY;
    int m_midClickX;
    int m_midClickY;
    float m_zoom;

    QVector3D m_rotCenter;
};

#endif /* ARCBALL_H_ */
