/*
 * navframe.cpp
 *
 * Created on: Jan 23, 2013
 * @author Ralph Schurade
 */
#include "navframe.h"
#include "navglwidget.h"

#include <QtWidgets>

NavFrame::NavFrame( QString name, int orient, QWidget *parent, const QGLWidget *shareWidget ) :
    QFrame( parent )
{
    //m_widget = new NavGLWidget( name, orient, parent, shareWidget );
    // workaround for qt5, with above line it creates the following error on startup
    // create: CreateWindowEx failed (Untergeordnetes Fenster kann nicht auf der obersten Ebene erstellt werden.)
    m_widget = new NavGLWidget( name, orient, NULL, shareWidget );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_widget );

    layout->setContentsMargins( 0, 0, 0, 0 );

    parent->show();
    setLayout( layout );

    setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

}

NavFrame::~NavFrame()
{
}

void NavFrame::update()
{
    m_widget->update();
}

void NavFrame::setWidgetVisible( bool visible )
{
    m_widget->setWidgetVisible( visible );
}
