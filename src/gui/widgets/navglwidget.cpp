/*
 * navglwidget.cpp
 *
 *  Created on: May 03, 2012
 *      Author: Ralph Schurade
 */

#include "navglwidget.h"

#include "../gl/navrendereraxial.h"
#include "../gl/navrenderercoronal.h"
#include "../gl/navrenderersagittal.h"

#include "../../data/models.h"

#include <QtGui>
#include <QDebug>

NavGLWidget::NavGLWidget( QString name, int orient, QWidget *parent, const QGLWidget *shareWidget ) :
	QGLWidget( parent, shareWidget )
{
    switch( orient )
    {
        case 0:
        {
            m_navRenderer = new NavRendererSagittal( name );
            break;
        }
        case 1:
        {
            m_navRenderer = new NavRendererCoronal( name );
            break;
        }
        case 2:
        {
            m_navRenderer = new NavRendererAxial( name );
            break;
        }
    }

    m_navRenderer->setModel( Models::g() );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

NavGLWidget::~NavGLWidget()
{
    destroy ( m_navRenderer );
}

QSize NavGLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize NavGLWidget::sizeHint() const
{
    return QSize( 2000, 2000 );
}

void NavGLWidget::initializeGL()
{
	m_navRenderer->initGL();
}

void NavGLWidget::paintGL()
{
    if ( m_visible )
    {
        m_navRenderer->draw();
    }
}

void NavGLWidget::resizeGL( int width, int height )
{
    m_navRenderer->resizeGL( width, height );
}

void NavGLWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_navRenderer->leftMouseDown( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_navRenderer->middleMouseDown( event->x(), event->y() );
    }
    updateGL();
}

void NavGLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_navRenderer->leftMouseDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_navRenderer->middleMouseDrag( event->x(), event->y() );
    }
    updateGL();
}

void NavGLWidget::wheelEvent(QWheelEvent *event)
{
     int numDegrees = event->delta() / 8;
     int numSteps = numDegrees / 15;
     m_navRenderer->mouseWheel( numSteps );
     updateGL();

}


void NavGLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void NavGLWidget::update()
{
    updateGL();
}

void NavGLWidget::setWidgetVisible( bool visible )
{
    m_visible = visible;
}
