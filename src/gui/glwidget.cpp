#include <QtGui/QtGui>

#include "../data/datastore.h"

#include "glwidget.h"

GLWidget::GLWidget( DataStore* dataStore, QWidget *parent ) :
	QGLWidget( QGLFormat( QGL::SampleBuffers ), parent ),
	m_dataStore( dataStore )
{
	m_sceneRenderer = new SceneRenderer( m_dataStore );

	connect( m_dataStore, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
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
    updateGL();
}

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_sceneRenderer->leftMouseDrag( event->x(), event->y() );
    }
    updateGL();
}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void GLWidget::update()
{
    updateGL();
}
