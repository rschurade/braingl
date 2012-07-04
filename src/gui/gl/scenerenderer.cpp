/*
 * scenerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include "../../data/datastore.h"

#include "arcball.h"
#include "slicerenderer.h"
#include "shrenderer.h"

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
    m_datasetSizeX = m_dataStore->data( m_dataStore->index( 0, 103 ), Qt::UserRole ).toFloat();
    m_datasetSizeY = m_dataStore->data( m_dataStore->index( 0, 104 ), Qt::UserRole ).toFloat();
    m_datasetSizeZ = m_dataStore->data( m_dataStore->index( 0, 105 ), Qt::UserRole ).toFloat();
    float dx = m_dataStore->data( m_dataStore->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = m_dataStore->data( m_dataStore->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = m_dataStore->data( m_dataStore->index( 0, 108 ), Qt::UserRole ).toFloat();
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

    pMatrix.translate( static_cast<float>( m_moveX ) / ( static_cast<float>( m_width ) / m_boundingbox ),
                       static_cast<float>( m_moveY ) / ( static_cast<float>( m_height ) / m_boundingbox ), 0 );

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
