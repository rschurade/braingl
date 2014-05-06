/*
 * fiberstipplingglwidget.cpp
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#include "fiberstipplingglwidget.h"

#include "../gl/fiberstipplingrenderer.h"

#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/datasets/datasettree.h"

#include <QtGui>
#include <QDebug>

#include "../core_3_3_context.h"

FiberStipplingGLWidget::FiberStipplingGLWidget( QString name, QWidget *parent, const QGLWidget *shareWidget ) :
    QGLWidget( new core_3_3_context(QGLFormat::defaultFormat()), parent, shareWidget ),
    m_visible( true )
{
    m_renderer = new FiberStipplingRenderer( "fiber stippling renderer" );

    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

FiberStipplingGLWidget::~FiberStipplingGLWidget()
{
    destroy ( m_renderer );
}

QSize FiberStipplingGLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize FiberStipplingGLWidget::sizeHint() const
{
    return QSize( 2000, 2000 );
}

void FiberStipplingGLWidget::initializeGL()
{
    // needed per OpenGL context and so per QGLWidget
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    m_renderer->initGL();
}

void FiberStipplingGLWidget::paintGL()
{
    if ( m_visible )
    {
        m_renderer->draw();
    }
}

void FiberStipplingGLWidget::resizeGL( int width, int height )
{
    m_renderer->resizeGL( width, height );
}

void FiberStipplingGLWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_renderer->middleMouseDown( event->x(), event->y() );
    }
    updateGL();
}

void FiberStipplingGLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_renderer->leftMouseDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_renderer->middleMouseDrag( event->x(), event->y() );
    }
    updateGL();
}

void FiberStipplingGLWidget::wheelEvent(QWheelEvent *event)
{
     int numDegrees = event->delta() / 8;
     int numSteps = numDegrees / 15;
     m_renderer->mouseWheel( numSteps );
     updateGL();

}


void FiberStipplingGLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void FiberStipplingGLWidget::update()
{
    if ( !m_visible )
    {
        return;
    }
    updateGL();
}

void FiberStipplingGLWidget::setWidgetVisible( bool visible )
{
    m_visible = visible;
}
