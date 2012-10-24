/*
 * arcball.cpp
 *
 *  Created on: 10.05.2012
 *      Author: Ralph
 */
#include <QtCore/QtDebug>

#include <math.h>

#include "arcball.h"

ArcBall::ArcBall( int width, int height ) :
    Epsilon( 0.00001 ),
    m_width( width ),
    m_height( height ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_oldMoveX( 0 ),
    m_oldMoveY( 0 ),
    m_midClickX( 0 ),
    m_midClickY( 0 ),
    m_zoom( 1.0f )
{
    m_adjust_width  = 1.0 / ((width - 1.0) * 0.5);
    m_adjust_height = 1.0 / ((height - 1.0) * 0.5);

    m_currentRot.setToIdentity();
    m_lastRot.setToIdentity();
}

ArcBall::~ArcBall()
{
}

// maps the specified mouse position to the sphere defined
// with center and radius. the resulting vector lies on the
// surface of the sphere.
QVector3D ArcBall::map_sphere( int x, int y )
{
    float tmpx = ( x * m_adjust_width ) - 1.0;
    float tmpy = 1.0 - ( y * m_adjust_height );

    float length = ( tmpx * tmpx ) + ( tmpy * tmpy );

    QVector3D bm;
    if ( length > 1.0 )
    {
        float norm = 1.0 / sqrt( length );
        bm.setX( tmpx * norm );
        bm.setY( tmpy * norm );
        bm.setZ( 0.0 );
    }
    else
    {
        bm.setX( tmpx );
        bm.setY( tmpy );
        bm.setZ( sqrt( 1.0 - length ) );
    }
    return bm;
}


/// sets the window size.
void ArcBall::set_win_size( int width,  int height )
{
    m_width = width;
    m_height = height;
    m_adjust_width = 1.0 / ( ( width - 1.0 ) * 0.5 );
    m_adjust_height = 1.0 / ( ( height - 1.0 ) * 0.5 );
}

/// sets the current position and calculates the current
/// rotation matrix.
void ArcBall::drag( int x, int y )
{
    QVector3D v_to = map_sphere( x, y );
    QVector3D perp = QVector3D::crossProduct( v_from, v_to);

    if ( perp.length() > Epsilon )
    {
        q_current_rotation.setX( perp.x() );
        q_current_rotation.setY( perp.y() );
        q_current_rotation.setZ( perp.z() );;
        q_current_rotation.setScalar( ( v_from.x() * v_to.x() ) + ( v_from.y() * v_to.y() ) + ( v_from.z() * v_to.z() ) );
    }
    else
    {
        q_current_rotation.setX( 0.0 );
        q_current_rotation.setY( 0.0 );
        q_current_rotation.setZ( 0.0 );
        q_current_rotation.setScalar( 0.0 );
    }
    m_currentRot.setToIdentity();
    m_currentRot.rotate( q_current_rotation );
    m_currentRot = m_currentRot * m_lastRot ;
}

/// indicates the beginning of the dragging.
void ArcBall::click( int x, int y )
{
    m_lastRot = m_currentRot;
    v_mouse_down.setX( x );
    v_mouse_down.setY( y );
    v_mouse_down.setZ( 0.0 );
    v_from = map_sphere( x, y );
}

void ArcBall::midClick( int x, int y )
{
    m_midClickX = x;
    m_midClickY = y;
    m_oldMoveX = m_moveX;
    m_oldMoveY = m_moveY;
}

void ArcBall::mouseWheel( int step )
{
    m_zoom += step;
    m_zoom = qMax( 1.0f, m_zoom );
}

void ArcBall::midDrag( int x, int y )
{
    m_moveX = ( m_oldMoveX + ( m_midClickX - x ) );
    m_moveY = ( m_oldMoveY + ( m_midClickY - y ) );
}

void ArcBall::setRotCenter( float x, float y, float z )
{
    m_rotCenter = QVector3D( -x, -y, -z );
}

void ArcBall::setView( int view )
{
    m_zoom = 1.0;
    m_moveX = 0;
    m_moveY = 0;
    m_oldMoveX = 0;
    m_oldMoveY = 0;
    m_currentRot.setToIdentity();
    m_lastRot.setToIdentity();

    QQuaternion rotx( sqrt(0.5), 0, 0, sqrt(0.5) );
    QQuaternion rot_x( -sqrt(0.5), 0, 0, sqrt(0.5) );
    QQuaternion roty( 0, sqrt(0.5), 0, sqrt(0.5) );
    QQuaternion rot_y( 0, -sqrt(0.5), 0, sqrt(0.5) );
    QQuaternion rotz( 0, 0, sqrt(0.5), sqrt(0.5) );

    if ( view == 2 )
    {
        m_currentRot.rotate( rotz );
        m_currentRot.rotate( rotx );
        m_currentRot.rotate( rotx );
    }
    if ( view == 3 )
    {
        m_currentRot.rotate( rot_x );
        m_currentRot.rotate( rot_y );
    }
}

/// returns the rotation matrix to be used directly
QMatrix4x4 ArcBall::getMVMat()
{
    QMatrix4x4 mv;
    mv.setToIdentity();

    mv = m_currentRot * mv ;

    m_zoom2 = m_zoom * m_zoom * 0.1 + 1.0;

    QVector3D halfMove( -m_moveX /1.7, m_moveY /1.7, 0 );
    QMatrix4x4 tmp;
    tmp.setToIdentity();
    tmp.translate( halfMove );
    tmp = tmp * m_currentRot;

    mv = mv + tmp;

    QVector3D scale( m_zoom2, m_zoom2, m_zoom2 );
    mv.scale( scale );

    mv.translate( m_rotCenter );

    return mv;
}

float ArcBall::getZoom()
{
    return m_zoom2;
}

float ArcBall::getMoveX()
{
    return m_moveX;
}

float ArcBall::getMoveY()
{
    return m_moveY;
}
