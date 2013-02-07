/*
 * scenerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include "glfunctions.h"

#include "scenerenderer.h"
#include "arcball.h"
#include "slicerenderer.h"

#include "../../data/datastore.h"
#include "../../data/enums.h"
#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"

#include <QtCore/QDebug>

#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

SceneRenderer::SceneRenderer( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QItemSelectionModel* roiSelectionModel ) :
    m_dataModel( dataModel ),
    m_globalModel( globalModel ),
    m_roiModel( roiModel ),
    m_roiSelectionModel( roiSelectionModel ),
    m_boundingbox( 200 ),
    m_nx( 160 ),
    m_ny( 200 ),
    m_nz( 160 ),
    m_width( 1 ),
    m_height( 1 ),
    m_ratio( 1.0 )
{
    m_sliceRenderer = new SliceRenderer( dataModel );
    m_sliceRenderer->setModel( globalModel );

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

    QColor color = m_globalModel->data( m_globalModel->index( (int)Fn::Global::BACKGROUND_COLOR_MAIN, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

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
    m_nx = m_globalModel->data( m_globalModel->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat();
    m_ny = m_globalModel->data( m_globalModel->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat();
    m_nz = m_globalModel->data( m_globalModel->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat();
    float dx = m_globalModel->data( m_globalModel->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = m_globalModel->data( m_globalModel->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = m_globalModel->data( m_globalModel->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();
    m_nx *= dx;
    m_ny *= dy;
    m_nz *= dz;

    m_arcBall->setRotCenter( m_nx / 2., m_ny / 2., m_nz / 2. );

    m_boundingbox = qMax ( m_nx, qMax( m_ny, m_nz ) );

    // Reset projection
    m_pMatrix.setToIdentity();

    float halfBB = m_boundingbox / 2.0;

    float bbx = m_boundingbox;
    float bby = m_boundingbox;

    if ( m_ratio >= 1.0 )
    {
        m_pMatrix.ortho( -halfBB * m_ratio, halfBB * m_ratio, -halfBB, halfBB, -3000, 3000 );
        bbx = m_boundingbox * m_ratio;
    }
    else
    {
        m_pMatrix.ortho( -halfBB, halfBB, -halfBB / m_ratio, halfBB / m_ratio, -3000, 3000 );
        bby = m_boundingbox / m_ratio;
    }

    m_mvMatrix = m_arcBall->getMVMat();
    m_mvMatrixInverse = m_mvMatrix.inverted();
    m_mvpMatrix = m_pMatrix * m_mvMatrix;

    m_globalModel->setData( m_globalModel->index( (int)Fn::Global::ZOOM, 0 ), m_arcBall->getZoom() );
    m_globalModel->setData( m_globalModel->index( (int)Fn::Global::MOVEX, 0 ), m_arcBall->getMoveX() );
    m_globalModel->setData( m_globalModel->index( (int)Fn::Global::MOVEY, 0 ), m_arcBall->getMoveY() );
    m_globalModel->setData( m_globalModel->index( (int)Fn::Global::BBX, 0 ), bbx );
    m_globalModel->setData( m_globalModel->index( (int)Fn::Global::BBY, 0 ), bby );

    //m_shRenderer->setSceneStats( m_arcBall->getZoom(), m_arcBall->getMoveX(), m_arcBall->getMoveY(), bbx, bby );
}

void SceneRenderer::draw()
{
    QColor color = m_globalModel->data( m_globalModel->index( (int)Fn::Global::BACKGROUND_COLOR_MAIN, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_sliceRenderer->draw( m_mvpMatrix );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glShadeModel( GL_SMOOTH );
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );

    renderDatasets();

    renderRois();
}

void SceneRenderer::renderDatasets()
{
    int countDatasets = m_dataModel->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataModel->index( i, (int)Fn::Property::ACTIVE );
        if ( m_dataModel->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            ds->draw( m_mvpMatrix, m_mvMatrixInverse, m_globalModel, m_roiModel );
        }
    }
}

void SceneRenderer::renderRois()
{
   int countTopBoxes = m_roiModel->rowCount();
   for ( int i = 0; i < countTopBoxes; ++i )
   {
       ROI* roi = VPtr<ROI>::asPtr( m_roiModel->data( m_roiModel->index( i, (int)Fn::ROI::POINTER ), Qt::DisplayRole ) );
       if ( roi->properties()->get( Fn::ROI::ACTIVE ).toBool() )
       {
           roi->draw( m_mvpMatrix, m_mvMatrixInverse );

           QModelIndex mi = m_roiModel->index( i, 0 );
           int countBoxes = m_roiModel->rowCount(  mi );

           for ( int k = 0; k < countBoxes; ++k )
           {
               roi = VPtr<ROI>::asPtr( m_roiModel->data( m_roiModel->index( k, (int)Fn::ROI::POINTER, mi ), Qt::DisplayRole ) );
               if ( roi->properties()->get( Fn::ROI::ACTIVE ).toBool() )
               {
                   roi->draw( m_mvpMatrix, m_mvMatrixInverse );
               }
           }
       }
   }
}

void SceneRenderer::setView( Fn::Orient view )
{
    m_arcBall->setView( (int)view );
    int countDatasets = m_dataModel->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataModel->index( i, (int)Fn::Property::ACTIVE );
        if ( m_dataModel->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            ds->properties()->set( Fn::Property::RENDER_SLICE, (int)view );
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

void SceneRenderer::rightMouseDown( int x, int y )
{
    // create offscreen texture
    GLFunctions::generate_frame_buffer_texture( m_width, m_height );
    // render
    GLFunctions::beginPicking();

    glEnable( GL_DEPTH_TEST );
    /* clear the frame buffer */
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //render_picking_scene();
    renderRois();

    // get id
    int pick = GLFunctions::get_object_id( x, y );
    //qDebug() << "picked object id: " << pick;

    if ( m_roiModel->rowCount() > 0 )
    {
        QModelIndex mi = m_roiModel->index( 0, (int)Fn::ROI::PICK_ID );
        QModelIndexList l = ( m_roiModel->match( mi, Qt::DisplayRole, pick ) );
        if ( l.size() > 0 )
        {
            m_roiSelectionModel->clear();
            m_roiSelectionModel->select( l.first(), QItemSelectionModel::Select );
        }
    }

    /* return to the default frame buffer */
    GLFunctions::endPicking();

    m_pickXold = x;
    m_pickYold = y;
}

void SceneRenderer::rightMouseDrag( int x, int y )
{
    QVector3D vs = mapMouse2World( x, y, 0 );
    QVector3D vs2 = mapMouse2World( m_pickXold, m_pickYold, 0 );
    QVector3D dir = vs - vs2;

    m_pickXold = x;
    m_pickYold = y;

    if ( m_roiSelectionModel->hasSelection() )
    {
        QModelIndex mi = m_roiSelectionModel->selectedIndexes().first();
        ROI* roi = VPtr<ROI>::asPtr( m_roiModel->data( m_roiModel->index( mi.row(), (int)Fn::ROI::POINTER, mi.parent() ), Qt::DisplayRole ) );
        float newx = roi->properties()->get( Fn::ROI::X ).toFloat() + dir.x();
        float newy = roi->properties()->get( Fn::ROI::Y ).toFloat() + dir.y();
        float newz = roi->properties()->get( Fn::ROI::Z ).toFloat() + dir.z();

        roi->properties()->set( Fn::ROI::X, newx );
        roi->properties()->set( Fn::ROI::Y, newy );
        roi->properties()->set( Fn::ROI::Z, newz );
        roi->properties()->slotPropChanged();
    }
}

QVector3D SceneRenderer::mapMouse2World( int x, int y, int dir )
{
    GLint viewport[4];
    //GLdouble modelview[16];
    //GLdouble projection[16];
    GLfloat winX, winY;



//  glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
//  glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    GLdouble posX, posY, posZ;
    gluUnProject( winX, winY, dir, m_mvMatrix.data(), m_pMatrix.data(), viewport, &posX, &posY, &posZ );

    QVector3D v( posX, posY, posZ );
    return v;
}
