/*
 * camera.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: schurade
 */

#include "camera.h"

#include "../../data/models.h"
#include "../../data/datasets/dataset.h"

Camera::Camera( int width, int height ) :
    CameraBase( width, height ),
    m_position( 0, 0, 500 ),
    m_lookAt( 0, 0, 0),
    m_up( 0, 1, 0 ),
    m_clickX( 0 ),
    m_clickY( 0 ),
    m_lookAtTemp( 0, 0, 0)
{
    m_keyboardStep = Models::getGlobal( Fn::Property::G_CAMERA_KEYBOARD_STEP ).toFloat();
}

Camera::~Camera()
{
}

void Camera::click( int x, int y )
{
    m_clickX = x;
    m_clickY = y;
    m_lookAtTemp = m_lookAt;
    m_positionTemp = m_position;
}

void Camera::drag( int x, int y )
{
    float distX = ( m_clickX - x );
    float distY = ( m_clickY - y );

    QVector3D dir = m_lookAt -  m_positionTemp;
    float distance = dir.length();
    dir.normalize();
    QVector3D dir2 = QVector3D::crossProduct( dir, m_up );
    dir2.normalize();

    m_position = m_positionTemp + distX * dir2 - distY * m_up;

    dir = m_lookAt -  m_position;
    dir.normalize();
    //m_lookAt = m_position + 100 * dir;
    m_position = m_lookAt - distance * dir;

    setGlobals();
}

void Camera::midClick( int x, int y )
{
}

void Camera::midDrag( int x, int y )
{
}

void Camera::mouseWheel( float step )
{
    if ( m_zoom < 2.0f )
    {
        step /= 2.0;
    }
    m_zoom += step;

    m_zoom = qMax( 1.0f, m_zoom );
}

void Camera::viewUp()
{
    getGlobals();
    m_lookAt = m_lookAt + m_keyboardStep * m_up;
    setGlobals();
}
void Camera::viewDown()
{
    getGlobals();
    m_lookAt = m_lookAt - m_keyboardStep * m_up;
    setGlobals();
}

void Camera::viewRight()
{
    getGlobals();
    QVector3D dir = m_lookAt -  m_position;
    dir.normalize();
    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_lookAt = m_lookAt + m_keyboardStep * dir;
    setGlobals();
}
void Camera::viewLeft()
{
    getGlobals();
    QVector3D dir = m_lookAt -  m_position;
    dir.normalize();
    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_lookAt = m_lookAt - m_keyboardStep * dir;
    setGlobals();
}

void Camera::left()
{
    getGlobals();
    float z = m_position.z();
    QVector3D dir = m_lookAt -  m_position;
    float distance = dir.length();
    dir.normalize();

    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_position = m_position + m_keyboardStep * dir;
    if ( Models::getGlobal( Fn::Property::G_CAMERA_FIXATE_Z).toBool() )
    {
        m_position.setZ( z );
    }
    dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_lookAt - distance * dir;

    setGlobals();
}
void Camera::right()
{
    getGlobals();
    float z = m_position.z();
    QVector3D dir = m_lookAt -  m_position;
    float distance = dir.length();
    dir.normalize();

    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_position = m_position - m_keyboardStep * dir;
    if ( Models::getGlobal( Fn::Property::G_CAMERA_FIXATE_Z).toBool() )
    {
        m_position.setZ( z );
    }
    dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_lookAt - distance * dir;

    setGlobals();
}

void Camera::up()
{
    getGlobals();
    QVector3D dir = m_lookAt -  m_position;
    float distance = dir.length();
    dir.normalize();

    m_position = m_position + m_keyboardStep * m_up;

    dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_lookAt - distance * dir;

    setGlobals();
}

void Camera::down()
{
    getGlobals();
    QVector3D dir = m_lookAt -  m_position;
    float distance = dir.length();
    dir.normalize();

    m_position = m_position - m_keyboardStep * m_up;

    dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_lookAt - distance * dir;

    setGlobals();
}

void Camera::forward()
{
    getGlobals();

    QVector3D dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_position + m_keyboardStep * dir;
    m_lookAt = m_lookAt + m_keyboardStep * dir;

    setGlobals();
}

void Camera::backward()
{
    getGlobals();

    QVector3D dir = m_lookAt - m_position;
    dir.normalize();
    m_position = m_position - m_keyboardStep * dir;
    m_lookAt = m_lookAt - m_keyboardStep * dir;

    setGlobals();
}

void Camera::strafeLeft()
{
    getGlobals();

    QVector3D dir = m_lookAt -  m_position;
    dir.normalize();
    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_position = m_position + m_keyboardStep * dir;
    m_lookAt = m_lookAt + m_keyboardStep * dir;

    setGlobals();
}
void Camera::strafeRight()
{
    getGlobals();
    QVector3D dir = m_lookAt -  m_position;
    dir.normalize();
    dir = QVector3D::crossProduct( dir, m_up );
    dir.normalize();
    m_position = m_position - m_keyboardStep * dir;
    m_lookAt = m_lookAt - m_keyboardStep * dir;

    setGlobals();
}

void Camera::setRotCenter( float x, float y, float z )
{
    m_lookAt.setX( x );
    m_lookAt.setY( y );
    m_lookAt.setZ( z );
}

QMatrix4x4 Camera::getMVMat()
{
    QMatrix4x4 out;
    out.setToIdentity();
    getGlobals();
    out.lookAt( m_position, m_lookAt, m_up );
    return out;
}

void Camera::setView( Fn::Orient view )
{
    if( !Models::d()->rowCount() == 0)
    {
        return;
    }
    Dataset* ds = Models::getDataset( 0 );
    QPair<QVector3D, QVector3D>bb = ds->getBoundingBox();
    QVector3D center = ( bb.first + bb.second ) / 2;

    m_lookAt = QVector3D( center.x(), center.y(), center.z() );
    m_up = QVector3D( 0, 0 , 1 );
    m_zoom = 1.0;
    switch( view )
    {
        case Fn::Orient::NONE:
            break;
        case Fn::Orient::AXIAL:
            m_position = QVector3D( center.x(), center.y(), center.z() + 200 );
            break;
        case Fn::Orient::CORONAL:
            m_position = QVector3D( center.x(), center.y() - 200, center.z() );
            break;
        case Fn::Orient::SAGITTAL:
            m_position = QVector3D( center.x() - 200, center.y(), center.z() );
            break;
        case Fn::Orient::AXIAL2:
            m_position = QVector3D( center.x(), center.y(), center.z() - 200 );
            break;
        case Fn::Orient::CORONAL2:
            m_position = QVector3D( center.x(), center.y() + 200, center.z() );
            break;
        case Fn::Orient::SAGITTAL2:
            m_position = QVector3D( center.x() + 200, center.y(), center.z() );
            break;
    }
    setGlobals();
}

float Camera::getMoveX()
{
    return 0;
}

float Camera::getMoveY()
{
    return 0;
}

void Camera::setMoveX( float x )
{

}

void Camera::setMoveY( float y )
{

}

QList<QVariant> Camera::getState()
{
    QList<QVariant> state;
    state.push_back( m_position );
    state.push_back( m_lookAt );
    state.push_back( m_up );
    return state;
}

void Camera::setState( QList<QVariant> state )
{
    m_position = state[0].value<QVector3D>();
    m_lookAt = state[1].value<QVector3D>();
    m_up = state[2].value<QVector3D>();
    setGlobals();
}

void Camera::getGlobals()
{
    m_position.setX( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_X, 0 ) ).toFloat() );
    m_position.setY( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_Y, 0 ) ).toFloat() );
    m_position.setZ( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_Z, 0 ) ).toFloat() );
    m_lookAt.setX( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_X, 0 ) ).toFloat() );
    m_lookAt.setY( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_Y, 0 ) ).toFloat() );
    m_lookAt.setZ( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_Z, 0 ) ).toFloat() );
    m_up.setX( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_X, 0 ) ).toFloat() );
    m_up.setY( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_Y, 0 ) ).toFloat() );
    m_up.setZ( Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_Z, 0 ) ).toFloat() );
    m_keyboardStep = Models::getGlobal( Fn::Property::G_CAMERA_KEYBOARD_STEP ).toFloat();
}

void Camera::setGlobals()
{
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_X, 0 ), m_position.x() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_Y, 0 ), m_position.y() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_POS_Z, 0 ), m_position.z() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_X, 0 ), m_lookAt.x() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_Y, 0 ), m_lookAt.y() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_LOOKAT_Z, 0 ), m_lookAt.z() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_X, 0 ), m_up.x() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_Y, 0 ), m_up.y() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CAMERA_UP_Z, 0 ), m_up.z() );
}
