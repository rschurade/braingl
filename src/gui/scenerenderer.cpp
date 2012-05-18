/*
 * scenerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include "../data/datastore.h"

#include "arcball.h"
#include "slicerenderer.h"

#include "scenerenderer.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

SceneRenderer::SceneRenderer( DataStore* dataStore ) :
        m_dataStore( dataStore ),
        m_ratio( 1.0 )
{
    m_sliceRenderer = new SliceRenderer();
    m_sliceRenderer->setModel( m_dataStore );

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
    GLenum errorCode = glewInit();
    if ( GLEW_OK != errorCode )
    {
        qDebug() << "Problem: glewInit failed, something is seriously wrong.";
        qDebug() << glewGetErrorString( errorCode );
        exit( false );
    }
    else
    {
        //qDebug() << glewGetErrorString( errorCode );
    }

    glClearColor( 1.0, 1.0, 0.0, 0.0 );

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

    m_sliceRenderer->init();
}

void SceneRenderer::resizeGL( int width, int height )
{
    m_arcBall->set_win_size( width, height );

    m_ratio = static_cast<float>( width )/ static_cast<float>(height);
    glViewport( 0, 0, width, height );

    m_thisRot = m_arcBall->getRotMat() *  m_lastRot;

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();
    pMatrix.ortho( -100 * m_ratio, 100 * m_ratio, -100, 100, -3000, 3000 );
    m_thisRot.translate( -80.0, -100.0, -80.0 );
    m_mvpMatrix = pMatrix * m_thisRot;
}

void SceneRenderer::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_sliceRenderer->draw( m_mvpMatrix );
}

void SceneRenderer::leftMouseDown( int x, int y )
{
    m_lastRot = m_thisRot;
    m_lastRot.translate( 80.0, 100.0, 80.0 );
    m_arcBall->click( x, y );
}

void SceneRenderer::leftMouseDrag( int x, int y )
{
    m_arcBall->drag( x, y );
    m_thisRot = m_arcBall->getRotMat() *  m_lastRot;

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();
    pMatrix.ortho( -100 * m_ratio, 100 * m_ratio, -100, 100, -3000, 3000 );
    m_thisRot.translate( -80.0, -100.0, -80.0 );
    m_mvpMatrix = pMatrix * m_thisRot;
}
