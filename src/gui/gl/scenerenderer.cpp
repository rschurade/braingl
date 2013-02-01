/*
 * scenerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include "scenerenderer.h"
#include "arcball.h"
#include "slicerenderer.h"

#include "../../data/datastore.h"
#include "../../data/enums.h"
#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"

#include <QtCore/QDebug>

#include <math.h>

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
    m_ratio( 1.0 )
{
    m_sliceRenderer = new SliceRenderer();
    m_sliceRenderer->setModel( m_dataStore );

    m_arcBall = new ArcBall( 400, 400 );

    m_mvMatrix.setToIdentity();
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

//    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
//    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
//    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//
//    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    static GLfloat lightPosition[ 4 ] =
    { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    calcMVPMatrix();
    m_sliceRenderer->init();
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

    m_arcBall->setRotCenter( m_datasetSizeX / 2., m_datasetSizeY / 2., m_datasetSizeZ / 2. );

    m_boundingbox = qMax ( m_datasetSizeX, qMax( m_datasetSizeY, m_datasetSizeZ ) );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    float halfBB = m_boundingbox / 2.0;

    float bbx = m_boundingbox;
    float bby = m_boundingbox;

    if ( m_ratio >= 1.0 )
    {
        pMatrix.ortho( -halfBB * m_ratio, halfBB * m_ratio, -halfBB, halfBB, -3000, 3000 );
        bbx = m_boundingbox * m_ratio;
    }
    else
    {
        pMatrix.ortho( -halfBB, halfBB, -halfBB / m_ratio, halfBB / m_ratio, -3000, 3000 );
        bby = m_boundingbox / m_ratio;
    }

    m_mvMatrix = m_arcBall->getMVMat();
    m_mvMatrixInverse = m_mvMatrix.inverted();
    m_mvpMatrix = pMatrix * m_mvMatrix;

    m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_ZOOM ), m_arcBall->getZoom(), Qt::UserRole );
    m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_MOVEX ), m_arcBall->getMoveX(), Qt::UserRole );
    m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_MOVEY ), m_arcBall->getMoveY(), Qt::UserRole );
    m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_BBX ), bbx, Qt::UserRole );
    m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_BBY ), bby, Qt::UserRole );

    //m_shRenderer->setSceneStats( m_arcBall->getZoom(), m_arcBall->getMoveX(), m_arcBall->getMoveY(), bbx, bby );
}

void SceneRenderer::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_sliceRenderer->draw( m_mvpMatrix );

    int countDatasets = m_dataStore->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataStore->index( i, FNPROP_ACTIVE );
        if ( m_dataStore->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( m_dataStore->data( m_dataStore->index( i, FNPROP_DATASET_POINTER ), Qt::DisplayRole ) );
            ds->draw( m_mvpMatrix, m_mvMatrixInverse, m_dataStore );
        }
    }
}

void SceneRenderer::setView( int view )
{
    m_arcBall->setView( view );
    int countDatasets = m_dataStore->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataStore->index( i, FNPROP_ACTIVE );
        if ( m_dataStore->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( m_dataStore->data( m_dataStore->index( i, FNPROP_DATASET_POINTER ), Qt::DisplayRole ) );
            ds->properties()->set( FNPROP_RENDER_SLICE, view );
        }
    }
}

void SceneRenderer::leftMouseDown( int x, int y )
{
    m_arcBall->click( x, y );
}

void SceneRenderer::leftMouseDrag( int x, int y )
{
    m_arcBall->drag( x, y );
    calcMVPMatrix();
}

void SceneRenderer::middleMouseDown( int x, int y )
{
    m_arcBall->midClick( x, y );
    calcMVPMatrix();
}

void SceneRenderer::middleMouseDrag( int x, int y )
{
    m_arcBall->midDrag( x, y );
    calcMVPMatrix();
}

void SceneRenderer::mouseWheel( int step )
{
    m_arcBall->mouseWheel( step );
    calcMVPMatrix();
}
