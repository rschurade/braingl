/*
 * fndockwidget.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: schurade
 */

#include "fndockwidget.h"

FNDockWidget::FNDockWidget( QString name, QWidget* parent ) :
    QDockWidget( name, parent )
{
    m_titleWidget = new QWidget();
}

FNDockWidget::~FNDockWidget()
{
}

void FNDockWidget::toggleTitleWidget()
{
    QWidget* tmp = titleBarWidget();
    setTitleBarWidget( m_titleWidget );
    m_titleWidget = tmp;
}
