/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../../data/datastore.h"
#include "../widgets/sliderwitheditint.h"


#include "../widgets/navglwidget.h"

#include "docknavglwidget.h"

DockNavGLWidget::DockNavGLWidget( DataStore* dataStore, QString name, int orient, QWidget* parent, const QGLWidget *shareWidget ) :
    QDockWidget( name, parent ),
    m_dataStore( dataStore ),
    m_name( name )
{
    setObjectName( QString("nav gl ") + name );

    m_glWidget = new NavGLWidget( dataStore, name, orient, this, shareWidget );
    m_glWidget->setToolTip( QString( "nav gl" ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* sliderLayout = new QHBoxLayout();
    m_slider = new SliderWithEditInt("");

    m_slider->setMin( 0 );
    m_slider->setMax( 200 );

    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );

    m_layout->addWidget( m_glWidget );
    m_layout->addLayout( m_slider );
    m_layout->addStretch( 0 );

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 50, 50 );

    settingChanged();

    connect( dataStore, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( settingChanged() ) );
    connect( this, SIGNAL( sliderChange( QString, QVariant ) ), dataStore, SLOT( setGlobal( QString, QVariant ) ) );
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
    emit( sliderChange( m_name, value ) );
    //qDebug() << value;
}

void DockNavGLWidget::settingChanged()
{
    QModelIndex mi;
    if  ( m_name == "sagittal")
    {
        mi = m_dataStore->index( 0, 100 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, 103 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
    }
    else if ( m_name == "coronal" )
    {
        mi = m_dataStore->index( 0, 101 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, 104 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
    }
    else if ( m_name == "axial" )
    {
        mi = m_dataStore->index( 0, 102 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_dataStore->index( 0, 105 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
    }
}
