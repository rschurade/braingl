/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */
#include "docknavglwidget.h"

#include "../widgets/navglwidget.h"
#include "../widgets/navframe.h"
#include "../widgets/controls/sliderwitheditint2.h"

#include "../../data/enums.h"

#include <QtGui/QtGui>

DockNavGLWidget::DockNavGLWidget( QAbstractItemModel* model, QString name, int orient, QWidget* parent, const QGLWidget *shareWidget ) :
    QWidget( parent ),
    m_name( name ),
    m_model( model )
{

    setObjectName( QString("nav gl ") + name );

    m_glWidget = new NavFrame( model, name, orient, this, shareWidget );
    m_glWidget->setToolTip( QString( "nav gl" ) );

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

    setLayout( m_layout );

    m_glWidget->setMinimumSize( 50, 50 );

    settingChanged();
    setContentsMargins( 1, 1, 1, 1 );

    connect( model, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( settingChanged() ) );
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
        m_model->setData( m_model->index( 0, FNGLOBAL_SAGITTAL ), value, Qt::UserRole );
    }
    else if ( m_name == "coronal" )
    {
        m_model->setData( m_model->index( 0, FNGLOBAL_CORONAL ), value, Qt::UserRole );
    }
    else if ( m_name == "axial" )
    {
        m_model->setData( m_model->index( 0, FNGLOBAL_AXIAL ), value, Qt::UserRole );
    }
}

void DockNavGLWidget::settingChanged()
{
    QModelIndex mi;
    if  ( m_name == "sagittal")
    {
        mi = m_model->index( 0, FNGLOBAL_SAGITTAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_model->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_model->index( 0, FNGLOBAL_MAX_SAGITTAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_model->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
    else if ( m_name == "coronal" )
    {
        mi = m_model->index( 0, FNGLOBAL_CORONAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_model->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_model->index( 0, FNGLOBAL_MAX_CORONAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_model->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
    else if ( m_name == "axial" )
    {
        mi = m_model->index( 0, FNGLOBAL_AXIAL );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_model->data( mi, Qt::UserRole ).toInt() );
        }
        mi = m_model->index( 0, FNGLOBAL_MAX_AXIAL );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_model->data( mi, Qt::UserRole ).toInt() - 1 );
        }
    }
}
