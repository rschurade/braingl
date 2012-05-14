#include <QtGui/QtGui>

#include "../data/datastore.h"
#include "navrenderer.h"

#include "navglwidget.h"

NavGLWidget::NavGLWidget( DataStore* dataStore, QString name, QWidget *parent, const QGLWidget *shareWidget ) :
	QGLWidget( parent, shareWidget )
{
    m_navRenderer = new NavRenderer( dataStore, name );
}

NavGLWidget::~NavGLWidget()
{
}

QSize NavGLWidget::minimumSizeHint() const
{
    return QSize( 200, 200 );
}

QSize NavGLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}

void NavGLWidget::initializeGL()
{
	m_navRenderer->initGL();
}

void NavGLWidget::paintGL()
{
	m_navRenderer->draw();
}

void NavGLWidget::resizeGL( int width, int height )
{
    m_navRenderer->resizeGL( width, height );
}

void NavGLWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        //m_sceneRenderer->leftMouseDown( event->x(), event->y() );
    }
    updateGL();
}

void NavGLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        //m_sceneRenderer->leftMouseDrag( event->x(), event->y() );
    }
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
