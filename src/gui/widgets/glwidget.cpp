/*
 * glwidget.h
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#include "glwidget.h"
#include "../gl/glfunctions.h"

#include "../../data/models.h"

#include <QtGui>

GLWidget::GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent ) :
    QGLWidget( QGLFormat( QGL::SampleBuffers ), parent )
{
    m_sceneRenderer = new SceneRenderer( name, roiSelectionModel );

    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

GLWidget::GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent, const QGLWidget *shareWidget ) :
        QGLWidget( parent, shareWidget )
{
    m_sceneRenderer = new SceneRenderer( name, roiSelectionModel );

    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize GLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}

void GLWidget::initializeGL()
{
    m_sceneRenderer->initGL();
    GLFunctions::initTextRenderer();
    GLFunctions::initShapeRenderer();
}

void GLWidget::paintGL()
{
    m_sceneRenderer->draw();
}

void GLWidget::resizeGL( int width, int height )
{
    m_sceneRenderer->resizeGL( width, height );
}

void GLWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_sceneRenderer->leftMouseDown( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_sceneRenderer->middleMouseDown( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        m_sceneRenderer->rightMouseDown( event->x(), event->y() );
    }
    updateGL();
}

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_sceneRenderer->leftMouseDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_sceneRenderer->middleMouseDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        m_sceneRenderer->rightMouseDrag( event->x(), event->y() );
    }
    updateGL();
}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void GLWidget::enterEvent( QEvent *event )
{
    updateGL();
}

void GLWidget::wheelEvent( QWheelEvent *event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_sceneRenderer->mouseWheel( numSteps );
    updateGL();
}

void GLWidget::update()
{
    m_sceneRenderer->calcMVPMatrix();
    updateGL();
}

void GLWidget::setView( Fn::Orient view )
{
    m_sceneRenderer->setView( view );
    update();
}

QImage* GLWidget::screenshot()
{
    return m_sceneRenderer->screenshot();
}
