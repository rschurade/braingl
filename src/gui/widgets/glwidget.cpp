#include "glwidget.h"

#include <QtGui/QtGui>

GLWidget::GLWidget( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QWidget *parent ) :
	QGLWidget( QGLFormat( QGL::SampleBuffers ), parent )
{
	m_sceneRenderer = new SceneRenderer( dataModel, globalModel );

	connect( dataModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
	connect( globalModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
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
    updateGL();
}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event)
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
