/*
 * TreeWidgetRenderer.cpp
 *
 * Created on: 17.09.2013
 * @author Ralph Schurade
 */
#include "treewidgetrenderer.h"

#include "../gl/glfunctions.h"

#include "../../algos/tree.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"

#include "../../data/datasets/datasettree.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

TreeWidgetRenderer::TreeWidgetRenderer( QString name ) :
    m_name( name ),
    m_zoom( 1 ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_moveXOld( 0 ),
    m_moveYOld( 0 ),
    m_middleDownX( 0 ),
    m_middleDownY( 0 )
{
}

TreeWidgetRenderer::~TreeWidgetRenderer()
{
}

void TreeWidgetRenderer::init()
{
}

void TreeWidgetRenderer::initGL()
{
    initializeOpenGLFunctions();
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glEnable( GL_MULTISAMPLE );
}

void TreeWidgetRenderer::draw()
{
    glViewport( 0, 0, m_width, m_height );

    QColor color = Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV2, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



   // Reset projection
   QMatrix4x4 pMatrix;
   pMatrix.setToIdentity();

    QList<Dataset*> dl = Models::getDatasets( Fn::DatasetType::TREE );
    if ( dl.empty() )
    {
        return;
    }

    DatasetTree* ds = dynamic_cast<DatasetTree*>( dl[0] );

    if ( m_name == "tree" )
    {
        int leaves = ds->getTree()->getNumLeaves();
        float zoom = qMin( leaves, m_width * ( m_zoom - 1 ) ) / 2;
        pMatrix.ortho(  0 - m_moveX + zoom,  leaves - m_moveX - zoom, 0, 1., -3000, 3000 );
        //pMatrix.ortho(  -500,  500, -500, 500, -3000, 3000 );
        ds->drawTree( pMatrix, m_width, m_height );
    }
    else if ( m_name == "tree2" )
    {
        int leaves = ds->getRoot()->getNumLeaves();
        float zoom = qMin( leaves, m_width * ( m_zoom - 1 ) ) / 2;
        pMatrix.ortho(  0 - m_moveX + zoom,  leaves - m_moveX - zoom, 0, 1., -3000, 3000 );
        //pMatrix.ortho(  -500,  500, -500, 500, -3000, 3000 );
        ds->drawRoot( pMatrix, m_width, m_height );
    }
}

void TreeWidgetRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;
}

void TreeWidgetRenderer::leftMouseDown( int x, int y )
{
}

void TreeWidgetRenderer::leftMouseDrag( int x, int y )
{
    leftMouseDown( x, y );
}

void TreeWidgetRenderer::setShaderVars()
{
}

void TreeWidgetRenderer::mouseWheel( int step )
{
    m_zoom += step;
    m_zoom = qMax( 1, m_zoom );
}

void TreeWidgetRenderer::middleMouseDown( int x, int y )
{
    m_moveXOld = m_moveX;
    m_moveYOld = m_moveY;
    m_middleDownX = x;
    m_middleDownY = y;
}

void TreeWidgetRenderer::middleMouseDrag( int x, int y )
{
    m_moveX = m_moveXOld - ( m_middleDownX - x ) * 10;
    m_moveY = m_moveYOld + m_middleDownY - y;
}
