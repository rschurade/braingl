#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <math.h>

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget( QWidget *parent ) :
        QGLWidget( QGLFormat( QGL::SampleBuffers ), parent )
{
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
    qglClearColor( QColor::fromCmykF(0.39, 0.39, 0.0, 0.0) );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
}

void GLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
}

void GLWidget::resizeGL( int width, int height )
{
    int side = qMin( width, height );
    glViewport( ( width - side ) / 2, ( height - side ) / 2, side, side );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho( -0.5, +0.5, -0.5, +0.5, 4.0, 15.0 );
#endif
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent( QMouseEvent *event )
{
}

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
}
