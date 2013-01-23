/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../../data/enums.h"
#include "../../data/datastore.h"
#include "../widgets/sliderwitheditint2.h"


#include "../widgets/navglwidget.h"
#include "../widgets/navframe.h"

#include "docknavglwidget.h"

DockNavGLWidget::DockNavGLWidget( DataStore* dataStore, QString name, int orient, QWidget* parent, const QGLWidget *shareWidget ) :
    QDockWidget( name, parent ),
    m_dataStore( dataStore ),
    m_name( name )
{
    setObjectName( QString("nav gl ") + name );

    m_glWidget = new NavFrame( dataStore, name, orient, this, shareWidget );
    m_glWidget->setToolTip( QString( "nav gl" ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();

    //QHBoxLayout* sliderLayout = new QHBoxLayout();
    m_slider = new SliderWithEditInt2();

    m_slider->setMin( 0 );
    m_slider->setMax( 200 );

    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );

    m_layout->addWidget( m_glWidget );
    m_layout->addWidget( m_slider );
    m_layout->addStretch( 0 );
    m_layout->setContentsMargins( 1, 1, 1, 1 );
    m_layout->setSpacing( 1 );

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 50, 50 );

    settingChanged();
    setContentsMargins( 0, 1, 0, 0 );

    connect( dataStore, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( settingChanged() ) );

    m_titleWidget = new QWidget(); /* where this a QMainWindow object */
}

DockNavGLWidget::~DockNavGLWidget()
{
}

QSize DockNavGLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize DockNavGLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}


void DockNavGLWidget::sliderChanged( int value )
{
    if  ( m_name == "sagittal")
    {
        m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_SAGITTAL ), value, Qt::UserRole );
    }
    else if ( m_name == "coronal" )
    {
        m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_CORONAL ), value, Qt::UserRole );
    }
    else if ( m_name == "axial" )
    {
        m_dataStore->setData( m_dataStore->index( 0, FNGLOBAL_AXIAL ), value, Qt::UserRole );
    }
}

void DockNavGLWidget::settingChanged()
{
    QModelIndex mi;
    if  ( m_name == "sagittal")
    {
        mi = m_dataStore->index( 0, FNGLOBAL_SAGITTAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, FNGLOBAL_MAX_SAGITTAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
    else if ( m_name == "coronal" )
    {
        mi = m_dataStore->index( 0, FNGLOBAL_CORONAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, FNGLOBAL_MAX_CORONAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
    else if ( m_name == "axial" )
    {
        mi = m_dataStore->index( 0, FNGLOBAL_AXIAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, FNGLOBAL_MAX_AXIAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
}

void DockNavGLWidget::toggleTitleWidget()
{
    QWidget* tmp = titleBarWidget();
    setTitleBarWidget( m_titleWidget );
    m_titleWidget = tmp;
}
