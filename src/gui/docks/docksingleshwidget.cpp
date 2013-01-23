/*
 * docksingleshwidget.cpp
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../../data/datastore.h"

#include "../widgets/singleshwidget.h"


#include "docksingleshwidget.h"

DockSingleSHWidget::DockSingleSHWidget( DataStore* dataStore, QString name, QWidget* parent, const QGLWidget *shareWidget ) :
    FNDockWidget( name, parent ),
    m_name( name )
{

    setObjectName( QString("nav gl ") + name );

    m_glWidget = new SingleSHWidget( dataStore, name, this, shareWidget );
    m_glWidget->setToolTip( QString( "single sh" ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();
    m_layout->addWidget( m_glWidget );

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 50,50 );
}

DockSingleSHWidget::~DockSingleSHWidget()
{
}

QSize DockSingleSHWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize DockSingleSHWidget::sizeHint() const
{
    return QSize( 400, 400 );
}
