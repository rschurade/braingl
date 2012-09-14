/*
 * scenerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include <math.h>

#include "../../data/datastore.h"
#include "../../data/enums.h"

#include "arcball.h"
#include "slicerenderer.h"
#include "shrenderer.h"
#include "tensorrenderer.h"

#include "scenerenderer.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

SceneRenderer::SceneRenderer( DataStore* dataStore ) :
    m_dataStore( dataStore ),
    m_boundingbox( 200 ),
    m_datasetSizeX( 160 ),
    m_datasetSizeY( 200 ),
    m_datasetSizeZ( 160 ),
    m_width( 1 ),
    m_height( 1 ),
    m_ratio( 1.0 ),
    m_zoom( 1 ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_moveXOld( 0 ),
    m_moveYOld( 0 )
{
    m_sliceRenderer = new SliceRenderer();
    m_sliceRenderer->setModel( m_dataStore );

    m_shRenderer = new SHRenderer();
    m_shRenderer->setModel( m_dataStore );

    m_tensorRenderer = new TensorRenderer();
    m_tensorRenderer->setModel( m_dataStore );

    m_arcBall = new ArcBall( 400, 400 );

    m_thisRot.setToIdentity();
    m_thisRot.translate( -80.0, -100.0, -80.0 );
    m_lastRot.setToIdentity();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::initGL()
{
    qDebug() << "gl init main widget";
    GLenum errorCode = glewInit();
    if ( GLEW_OK != errorCode )
    {
        qDebug() << "Problem: glewInit failed, something is seriously wrong.";
        qDebug() << glewGetErrorString( errorCode );
        exit( false );
    }
    else
    {
        qDebug() << "OpenGL initialized.";
    }

    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] =
    { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    calcMVPMatrix();
    m_sliceRenderer->init();
    m_shRenderer->init();
    m_tensorRenderer->init();
}

void SceneRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;
    m_arcBall->set_win_size( width, height );

    m_ratio = static_cast<float>( width )/ static_cast<float>(height);
    glViewport( 0, 0, width, height );

    calcMVPMatrix();
}

void SceneRenderer::calcMVPMatrix()
{
    m_datasetSizeX = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_MAX_SAGITTAL ), Qt::UserRole ).toFloat();
    m_datasetSizeY = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toFloat();
    m_datasetSizeZ = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_MAX_AXIAL ), Qt::UserRole ).toFloat();
    float dx = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_SLICE_DX ), Qt::UserRole ).toFloat();
    float dy = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_SLICE_DY ), Qt::UserRole ).toFloat();
    float dz = m_dataStore->data( m_dataStore->index( 0, FNGLOBAL_SLICE_DZ ), Qt::UserRole ).toFloat();
    m_datasetSizeX *= dx;
    m_datasetSizeY *= dy;
    m_datasetSizeZ *= dz;

    m_boundingbox = qMax ( m_datasetSizeX, qMax( m_datasetSizeY, m_datasetSizeZ ) );

    m_thisRot = m_arcBall->getRotMat() *  m_lastRot;
    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();
    pMatrix.ortho( 0, m_boundingbox, 0, m_boundingbox, -3000, 3000 );
    pMatrix.translate( m_boundingbox / 2, m_boundingbox / 2, 0 );

    pMatrix.translate( static_cast<float>( m_moveX * m_zoom ) / ( static_cast<float>( m_width ) / m_boundingbox ),
                       static_cast<float>( m_moveY * m_zoom ) / ( static_cast<float>( m_height ) / m_boundingbox ), 0 );

    pMatrix.scale( m_zoom );

    if ( m_ratio >= 1.0 )
    {
        pMatrix.scale( 1.0 / m_ratio, 1.0, 1.0 );
    }
    else
    {
        pMatrix.scale( 1.0, m_ratio, 1.0 );
    }

    m_thisRot.translate( m_datasetSizeX / -2, m_datasetSizeY / -2, m_datasetSizeZ / -2 );
    m_mvpMatrix = pMatrix * m_thisRot;
}

void SceneRenderer::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_sliceRenderer->draw( m_mvpMatrix );
    m_shRenderer->draw( m_mvpMatrix );
    m_tensorRenderer->draw( m_mvpMatrix );
}

void SceneRenderer::setView( int view )
{
    m_zoom = 1.0;
    m_moveX = 0;
    m_moveY = 0;
    m_moveXOld = 0;
    m_moveYOld = 0;
    m_thisRot.setToIdentity();
    m_lastRot.setToIdentity();
    m_arcBall = new ArcBall( m_width, m_height);

    QQuaternion rotx( sqrt(0.5), 0, 0, sqrt(0.5) );
    QQuaternion rot_x( -sqrt(0.5), 0, 0, sqrt(0.5) );
    QQuaternion roty( 0, sqrt(0.5), 0, sqrt(0.5) );
    QQuaternion rot_y( 0, -sqrt(0.5), 0, sqrt(0.5) );
    QQuaternion rotz( 0, 0, sqrt(0.5), sqrt(0.5) );

    if ( view == 2 )
    {
        m_lastRot.rotate( rotz );
        m_lastRot.rotate( rotx );
        m_lastRot.rotate( rotx );
    }
    if ( view == 3 )
    {
        m_lastRot.rotate( rot_x );
        m_lastRot.rotate( rot_y );
    }
}

void SceneRenderer::leftMouseDown( int x, int y )
{
    m_lastRot = m_thisRot;
    m_lastRot.translate( m_datasetSizeX / 2, m_datasetSizeY / 2, m_datasetSizeZ / 2 );
    m_arcBall->click( x, y );
}

void SceneRenderer::leftMouseDrag( int x, int y )
{
    m_arcBall->drag( x, y );
    calcMVPMatrix();
}

void SceneRenderer::middleMouseDown( int x, int y )
{
    m_moveXOld = m_moveX;
    m_moveYOld = m_moveY;
    m_middleDownX = x;
    m_middleDownY = y;
}

void SceneRenderer::middleMouseDrag( int x, int y )
{
    m_moveX = m_moveXOld - ( m_middleDownX - x );
    m_moveY = m_moveYOld + m_middleDownY - y;
    calcMVPMatrix();
}

void SceneRenderer::mouseWheel( int step )
{
    m_zoom += step;
    m_zoom = qMax( 1, m_zoom );

    calcMVPMatrix();
}
