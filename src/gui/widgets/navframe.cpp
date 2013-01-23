/*
 * navframe.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: schurade
 */
#include <QtGui/QtGui>

#include "../../data/datastore.h"
#include "navglwidget.h"
#include "navframe.h"



NavFrame::NavFrame( DataStore* dataStore, QString name, int orient, QWidget *parent, const QGLWidget *shareWidget ) :
    QFrame( parent )
{
    m_widget = new NavGLWidget( dataStore, name, orient, parent, shareWidget );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_widget );

    layout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( layout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

}

NavFrame::~NavFrame()
{
    // TODO Auto-generated destructor stub
}

