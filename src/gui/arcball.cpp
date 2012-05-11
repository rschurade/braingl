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
    m_height( height )
{
    m_adjust_width  = 1.0 / ((width - 1.0) * 0.5);
    m_adjust_height = 1.0 / ((height - 1.0) * 0.5);

    m_currentRot.setToIdentity();
}

ArcBall::~ArcBall()
{
}

// maps the specified mouse position to the sphere defined
// with center and radius. the resulting vector lies on the
// surface of the sphere.
QVector3D ArcBall::map_sphere( int x, int y )
{
#if 1
    float tmpx = 1.0 - ( x * m_adjust_width );
    float tmpy = ( y * m_adjust_height ) - 1.0;
#else
    float tmpx = ( x * m_adjust_width ) - 1.0;
    float tmpy = 1.0 - ( y * m_adjust_height );
#endif

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
}

/// indicates the beginning of the dragging.
void ArcBall::click( int x, int y )
{
    v_mouse_down.setX( x );
    v_mouse_down.setY( y );
    v_mouse_down.setZ( 0.0 );
    v_from = map_sphere( x, y );
}

void ArcBall::release()
{
}

// returns the rotation matrix to be used directly
QQuaternion ArcBall::getRotQuat()
{
    return q_current_rotation;
}


/// returns the rotation matrix to be used directly
QMatrix4x4 ArcBall::getRotMat()
{
    return m_currentRot;
}

