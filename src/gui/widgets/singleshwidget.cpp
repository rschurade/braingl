/*
 * singleshwidget.cpp
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */

#include <QtGui/QtGui>

#include "../../data/datastore.h"
#include "../gl/singleshrenderer.h"

#include "singleshwidget.h"

SingleSHWidget::SingleSHWidget( DataStore* dataStore, QString name, QWidget *parent, const QGLWidget *shareWidget ) :
    QGLWidget( parent, shareWidget )
{
    m_renderer = new SingleSHRenderer( name );
    m_renderer->setModel( dataStore );
    connect( dataStore, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
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
    updateGL();
}
