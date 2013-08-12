/*
 * camerabase.h
 *
 *  Created on: Aug 7, 2013
 *      Author: schurade
 */

#ifndef CAMERABASE_H_
#define CAMERABASE_H_

#include "../../data/enums.h"

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

class CameraBase
{
public:
    CameraBase( int width, int height );
    virtual ~CameraBase();

    virtual void set_win_size( int width,  int height );

    virtual void click( int x, int y ) = 0;
    virtual void drag( int x, int y ) = 0;

    virtual void midClick( int x, int y ) = 0;
    virtual void midDrag( int x, int y ) = 0;

    virtual void mouseWheel( float step ) = 0;

    virtual void setRotCenter( float x, float y, float z ) = 0;

    virtual QMatrix4x4 getMVMat() = 0;

    virtual void setView( Fn::Orient view ) = 0;

    virtual float getZoom();
    virtual float getMoveX() = 0;
    virtual float getMoveY() = 0;

    virtual QList<QVariant>getState() = 0;
    virtual void setState( QList<QVariant> state ) = 0;

protected:
    int m_width;  // width of window
    int m_height; // height of window
    float m_adjust_width;
    float m_adjust_height;
    float m_zoom;

private:

};

#endif /* CAMERABASE_H_ */
