/*
 * singleshwidget.cpp
 *
 * Created on: 12.07.2012
 * @author Ralph Schurade
 */
#include "singleshwidget.h"
#include "../gl/glfunctions.h"

#include "../gl/singleshrenderer.h"

#include "../../data/models.h"

#include <QtGui>

#include "../core_3_3_context.h"

SingleSHWidget::SingleSHWidget( QString name, QWidget *parent, const QGLWidget *shareWidget ) :
    QGLWidget( new core_3_3_context(QGLFormat::defaultFormat()), parent, shareWidget ),
    m_visible( false )
{
    m_renderer = new SingleSHRenderer();
    m_renderer->setModel( Models::d() );
    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

SingleSHWidget::~SingleSHWidget()
{
    // TODO Auto-generated destructor stub
}

QSize SingleSHWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize SingleSHWidget::sizeHint() const
{
    return QSize( 400, 400 );
}

void SingleSHWidget::initializeGL()
{
    // needed per OpenGL context and so per QGLWidget
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    m_renderer->initGL();
}

void SingleSHWidget::paintGL()
{
    m_renderer->draw();
}

void SingleSHWidget::resizeGL( int width, int height )
{
    m_renderer->resizeGL( width, height );
}

void SingleSHWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_renderer->leftMouseDown( event->x(), event->y() );
    }
    updateGL();
}

void SingleSHWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_renderer->leftMouseDrag( event->x(), event->y() );
    }
    updateGL();
}

void SingleSHWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void SingleSHWidget::update()
{
    if ( !m_visible )
    {
        return;
    }
    updateGL();
}

void SingleSHWidget::visibilityChanged( bool visible )
{
    m_visible = visible;
}
