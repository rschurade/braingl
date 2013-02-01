/*
 * navframe.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: schurade
 */
#include "navframe.h"
#include "navglwidget.h"

#include "../../data/datastore.h"

#include <QtGui/QtGui>

NavFrame::NavFrame( DataStore* dataStore, QString name, int orient, QWidget *parent, const QGLWidget *shareWidget ) :
    QFrame( parent )
{
    m_widget = new NavGLWidget( dataStore, name, orient, parent, shareWidget );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_widget );

    layout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( layout );

    setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

}

NavFrame::~NavFrame()
{
    // TODO Auto-generated destructor stub
}

