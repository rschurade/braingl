/*
 * navframe.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Ralph Schurade
 */
#include "navframe.h"
#include "navglwidget.h"

#include <QtGui/QtGui>

NavFrame::NavFrame( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QString name, int orient, QWidget *parent, const QGLWidget *shareWidget ) :
    QFrame( parent )
{
    m_widget = new NavGLWidget( dataModel, globalModel, name, orient, parent, shareWidget );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_widget );

    layout->setContentsMargins( 0, 0, 0, 0 );

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
