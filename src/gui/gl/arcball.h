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

    void drag( int x, int y );

    void click( int x, int y );

    QQuaternion getRotQuat();
    QMatrix4x4 getRotMat();

private:
    QVector3D map_sphere( int x, int y );

    float Epsilon;
    int m_width;  // width of window
    int m_height; // height of window
    float m_adjust_width;
    float m_adjust_height;

    QVector3D v_mouse_current;      // mouse position at the beginning of dragging
    QVector3D v_mouse_down;         // mouse position at the beginning of dragging
    QQuaternion q_current_rotation; // current rotation
    QMatrix4x4 m_currentRot;               // current rotation matrix

    QVector3D v_from;
};

#endif /* ARCBALL_H_ */
