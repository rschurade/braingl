/*
 * camera.h
 *
 *  Created on: Aug 7, 2013
 *      Author: schurade
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "camerabase.h"

#include <QVector3D>
#include <QMatrix4x4>

class Camera : public CameraBase
{
public:
    Camera( int width, int height );
    virtual ~Camera();

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

    void viewUp();
    void viewDown();
    void viewLeft();
    void viewRight();
    void forward();
    void backward();
    void up();
    void down();
    void strafeLeft();
    void strafeRight();

    QList<QVariant>getState();
    void setState( QList<QVariant> state );

private:
    void getGlobals();
    void setGlobals();

    QVector3D m_position;
    QVector3D m_lookAt;
    QVector3D m_up;

    int m_clickX;
    int m_clickY;
    QVector3D m_lookAtTemp;
};

#endif /* CAMERA_H_ */
