/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../data/datastore.h"

#include "navglwidget.h"

#include "docknavglwidget.h"

DockNavGLWidget::DockNavGLWidget( DataStore* dataStore, QString name, QWidget* parent, const QGLWidget *shareWidget ) :
    QDockWidget( name, parent ),
    m_dataStore( dataStore ),
    m_name( name )
{
    setObjectName( QString("nav gl ") + name );

    m_glWidget = new NavGLWidget( dataStore, name, this, shareWidget );
    m_glWidget->setToolTip( QString( "nav gl" ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* sliderLayout = new QHBoxLayout();
    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );
    m_slider->setMinimum( 0 );
    m_slider->setMaximum( 200 );

    m_lineEdit = new QLineEdit();
    m_lineEdit->setMaxLength( 3 );
    m_lineEdit->setMaximumWidth( 30 );
    connect( m_slider, SIGNAL( valueChanged( int) ), this, SLOT( sliderChanged( int ) ) );

    sliderLayout->addWidget( m_lineEdit );
    sliderLayout->addWidget( m_slider );

    m_layout->addWidget( m_glWidget );
    m_layout->addLayout( sliderLayout );
    m_layout->addStretch( 0 );

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 200, 200 );
    m_glWidget->setMaximumSize( 800, 800 );

    settingChanged();

    connect( dataStore, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( this, SIGNAL( sliderChange( QString, QVariant ) ), dataStore, SLOT( setGlobal( QString, QVariant ) ) );
}

DockNavGLWidget::~DockNavGLWidget()
{
}

QSize DockNavGLWidget::minimumSizeHint() const
{
    return QSize( 200, 200 );
}

QSize DockNavGLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}


void DockNavGLWidget::sliderChanged( int value )
{
    m_lineEdit->setText( QString::number( value, 10 ) );
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
            m_slider->setMaximum( m_dataStore->data( mi, Qt::UserRole ).toInt() );
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
            m_slider->setMaximum( m_dataStore->data( mi, Qt::UserRole ).toInt() );
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
            m_slider->setMaximum( m_dataStore->data( mi, Qt::UserRole ).toInt() );
        }
    }
}

void DockNavGLWidget::update()
{
    settingChanged();
    m_glWidget->update();
}
