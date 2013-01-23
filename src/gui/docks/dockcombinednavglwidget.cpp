/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../../data/datastore.h"

#include "../widgets/combinednavglwidget.h"

#include "dockcombinednavglwidget.h"

DockCombinedNavGLWidget::DockCombinedNavGLWidget( DataStore* dataStore, QString name, QWidget* parent, const QGLWidget *shareWidget ) :
    FNDockWidget( name, parent ),
    m_name( name )
{
    setObjectName( QString("nav gl ") + name );

    m_glWidget = new CombinedNavGLWidget( dataStore, name, this, shareWidget );
    m_glWidget->setToolTip( QString( "nav gl" ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();
    m_layout->addWidget( m_glWidget );

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 50,50 );
}

DockCombinedNavGLWidget::~DockCombinedNavGLWidget()
{
}

QSize DockCombinedNavGLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize DockCombinedNavGLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}
