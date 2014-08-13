/*
 * arcball.cpp
 *
 * Created on: 10.05.2012
 * @author Ralph Schurade
 */
#include "arcball.h"

#include "../../data/models.h"

#include <QtDebug>

#include <math.h>

ArcBall::ArcBall( int width, int height ) :
    CameraBase( width, height ),
    Epsilon( 0.00001 ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_oldMoveX( 0 ),
    m_oldMoveY( 0 ),
    m_midClickX( 0 ),
    m_midClickY( 0 )
{

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

void ArcBall::mouseWheel( float step )
{
    if ( step < 0 )
    {
        m_zoom *= 1.1;
    }
    else
    {
        m_zoom /= 1.1;
        m_zoom = qMax( 0.2f, m_zoom );
    }
}

void ArcBall::midDrag( int x, int y )
{
    m_moveX = m_oldMoveX + ( ( m_midClickX - x ) / 2 ) / m_zoom;
    m_moveY = m_oldMoveY + ( ( m_midClickY - y ) / 2 ) / m_zoom;
}

void ArcBall::setRotCenter( float x, float y, float z )
{
    m_rotCenter = QVector3D( -x, -y, -z );
}

void ArcBall::setView( Fn::Orient view )
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
    QQuaternion rot_z( 0, 0, -sqrt(0.5), sqrt(0.5) );

    switch( view )
    {
        case Fn::Orient::NONE:
            break;
        case Fn::Orient::AXIAL:
            break;
        case Fn::Orient::CORONAL:
            m_currentRot.rotate( rotz );
            m_currentRot.rotate( rotx );
            m_currentRot.rotate( rotx );
            break;
        case Fn::Orient::SAGITTAL:
            m_currentRot.rotate( rot_x );
            m_currentRot.rotate( rot_y );
            break;
        case Fn::Orient::AXIAL2:
            m_currentRot.rotate( rot_y );
            m_currentRot.rotate( roty );
            break;
        case Fn::Orient::CORONAL2:
            m_currentRot.rotate( rotz );
            break;
        case Fn::Orient::SAGITTAL2:
            m_currentRot.rotate( rotx );
            m_currentRot.rotate( roty );
            break;
    }
}

/// returns the rotation matrix to be used directly
QMatrix4x4 ArcBall::getMVMat()
{
    QMatrix4x4 mv;
    mv.setToIdentity();

    mv = m_currentRot * mv ;

    float dist = Models::getGlobal( Fn::Property::G_ARCBALL_DISTANCE ).toFloat();

    QVector3D halfMove( -m_moveX, m_moveY, -dist );
    QMatrix4x4 tmp;
    tmp.setToIdentity();
    tmp.translate( halfMove );
    tmp = tmp * m_currentRot;

    mv = mv + tmp;

    mv.translate( m_rotCenter );

    return mv;
}

float ArcBall::getMoveX()
{
    return m_moveX;
}

float ArcBall::getMoveY()
{
    return m_moveY;
}

void ArcBall::setMoveX( float x )
{
    m_moveX = x;
}

void ArcBall::setMoveY( float y )
{
    m_moveY = y;
}

QList<QVariant> ArcBall::getState()
{
    QList<QVariant> state;
    state.push_back( "currentRot" );
    state.push_back( m_currentRot );
    state.push_back( "lastRot" );
    state.push_back( m_lastRot );
    state.push_back( "moveX" );
    state.push_back( m_moveX );
    state.push_back( "moveY" );
    state.push_back( m_moveY );
    state.push_back( "oldMoveX" );
    state.push_back( m_oldMoveX );
    state.push_back( "oldMoveY" );
    state.push_back( m_oldMoveY );
    state.push_back( "midClickX" );
    state.push_back( m_midClickX );
    state.push_back( "midClickY" );
    state.push_back( m_midClickY );
    state.push_back( "zoom" );
    state.push_back( m_zoom );
    state.push_back( "rotCenter" );
    state.push_back( m_rotCenter );
    state.push_back( "current_rotation" );
    state.push_back( q_current_rotation );
    return state;
}

void ArcBall::setState( QList<QVariant> state )
{
    m_currentRot = state[0].value<QMatrix4x4>();
    m_lastRot = state[1].value<QMatrix4x4>();
    m_moveX = state[2].toInt();
    m_moveY = state[3].toInt();
    m_oldMoveX = state[4].toInt();
    m_oldMoveY = state[5].toInt();
    m_midClickX = state[6].toInt();
    m_midClickY = state[7].toInt();
    m_zoom = state[8].toFloat();
    m_rotCenter = state[9].value<QVector3D>();
    q_current_rotation = state[10].value<QQuaternion>();
}

void ArcBall::setState( QMap<QString, QVariant> state )
{
    m_currentRot = state["currentRot"].value<QMatrix4x4>();
    m_lastRot = state["lastRot"].value<QMatrix4x4>();
    m_moveX = state["moveX"].toInt();
    m_moveY = state["moveY"].toInt();
    m_oldMoveX = state["oldMoveX"].toInt();
    m_oldMoveY = state["oldMoveY"].toInt();
    m_midClickX = state["midClickX"].toInt();
    m_midClickY = state["midClickY"].toInt();
    m_zoom = state["zoom"].toFloat();
    m_rotCenter = state["rotCenter"].value<QVector3D>();
    q_current_rotation = state["current_rotation"].value<QQuaternion>();
}


QQuaternion ArcBall::getRotation()
{
    return mat2quat( m_currentRot );
}

void ArcBall::setRotation( QQuaternion quat )
{
    m_currentRot.setToIdentity();
    m_lastRot.setToIdentity();
    m_currentRot.rotate( quat );
}

inline float SIGN(float x) {return (x >= 0.0f) ? +1.0f : -1.0f;}
inline float NORM(float a, float b, float c, float d) {return sqrt(a * a + b * b + c * c + d * d);}

QQuaternion ArcBall::mat2quat( QMatrix4x4 &mat )
{
    double r11 = mat( 0, 0 );
    double r12 = mat( 0, 1 );
    double r13 = mat( 0, 2 );
    double r21 = mat( 1, 0 );
    double r22 = mat( 1, 1 );
    double r23 = mat( 1, 2 );
    double r31 = mat( 2, 0 );
    double r32 = mat( 2, 1 );
    double r33 = mat( 2, 2 );

    double q0 = ( r11 + r22 + r33 + 1.0f ) / 4.0f;
    double q1 = ( r11 - r22 - r33 + 1.0f ) / 4.0f;
    double q2 = ( -r11 + r22 - r33 + 1.0f ) / 4.0f;
    double q3 = ( -r11 - r22 + r33 + 1.0f ) / 4.0f;

    if ( q0 < 0.0f )
        q0 = 0.0f;
    if ( q1 < 0.0f )
        q1 = 0.0f;
    if ( q2 < 0.0f )
        q2 = 0.0f;
    if ( q3 < 0.0f )
        q3 = 0.0f;
    q0 = sqrt( q0 );
    q1 = sqrt( q1 );
    q2 = sqrt( q2 );
    q3 = sqrt( q3 );
    if ( q0 >= q1 && q0 >= q2 && q0 >= q3 )
    {
        q0 *= +1.0f;
        q1 *= SIGN( r32 - r23 );
        q2 *= SIGN( r13 - r31 );
        q3 *= SIGN( r21 - r12 );
    }
    else if ( q1 >= q0 && q1 >= q2 && q1 >= q3 )
    {
        q0 *= SIGN( r32 - r23 );
        q1 *= +1.0f;
        q2 *= SIGN( r21 + r12 );
        q3 *= SIGN( r13 + r31 );
    }
    else if ( q2 >= q0 && q2 >= q1 && q2 >= q3 )
    {
        q0 *= SIGN( r13 - r31 );
        q1 *= SIGN( r21 + r12 );
        q2 *= +1.0f;
        q3 *= SIGN( r32 + r23 );
    }
    else if ( q3 >= q0 && q3 >= q1 && q3 >= q2 )
    {
        q0 *= SIGN( r21 - r12 );
        q1 *= SIGN( r31 + r13 );
        q2 *= SIGN( r32 + r23 );
        q3 *= +1.0f;
    }
    else
    {
        printf( "coding error\n" );
    }
    double r = NORM( q0, q1, q2, q3 );
    q0 /= r;
    q1 /= r;
    q2 /= r;
    q3 /= r;

    return QQuaternion( q0, q1, q2, q3 );
}

QVector3D ArcBall::getRotCenter()
{
    return m_rotCenter;
}
