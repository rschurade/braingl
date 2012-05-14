/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "navglwidget.h"

#include "docknavglwidget.h"

DockNavGLWidget::DockNavGLWidget( DataStore* dataStore, QString name, QWidget* parent, const QGLWidget *shareWidget ) :
    QDockWidget( name, parent ),
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

    panel->setLayout( m_layout );
    setWidget( panel );

    m_glWidget->setMinimumSize( 50,50 );

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
    m_lineEdit->setText( QString::number( value, 10 ) );
    emit( sliderChange( m_name, value ) );
    //qDebug() << value;
}

void DockNavGLWidget::settingChanged( QString name, QVariant data )
{
    if ( name == tr("max_") + m_name )
    {
        m_slider->setMaximum( data.toInt() );
    }
    if ( name == m_name )
    {
        m_slider->setValue( data.toInt() );
    }
    m_glWidget->update();
}
