/*
 * navglwidget.cpp
 *
 *  Created on: 12.05.2012
 *      Author: Ralph Schurade
 */
#include "docknavglwidget.h"

#include "../widgets/navglwidget.h"
#include "../widgets/navframe.h"
#include "../widgets/controls/sliderwitheditint2.h"

#include "../../data/enums.h"

#include <QtGui>

DockNavGLWidget::DockNavGLWidget( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QString name, int orient, QWidget* parent, const QGLWidget *shareWidget ) :
    QWidget( parent ),
    m_name( name ),
    m_globalModel( globalModel )
{

    setObjectName( QString("nav gl ") + name );

    m_glWidget = new NavFrame( dataModel, globalModel, name, orient, this, shareWidget );
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

    connect( m_globalModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( settingChanged() ) );
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
        m_globalModel->setData( m_globalModel->index( (int)Fn::Global::SAGITTAL, 0 ), value );
    }
    else if ( m_name == "coronal" )
    {
        m_globalModel->setData( m_globalModel->index( (int)Fn::Global::CORONAL, 0 ), value );
    }
    else if ( m_name == "axial" )
    {
        m_globalModel->setData( m_globalModel->index( (int)Fn::Global::AXIAL, 0 ), value );
    }
}

void DockNavGLWidget::settingChanged()
{
    QModelIndex mi;
    mi = m_globalModel->index( (int)Fn::Global::SHOW_NAV_SLIDERS, 0 );
    if ( m_globalModel->data( mi ).toBool() )
    {
        m_slider->show();
    }
    else
    {
        m_slider->hide();
    }

    if  ( m_name == "sagittal")
    {
        mi = m_globalModel->index( (int)Fn::Global::SAGITTAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_globalModel->data( mi ).toInt() );
        }
        mi = m_globalModel->index( (int)Fn::Global::MAX_SAGITTAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_globalModel->data( mi ).toInt() - 1 );
        }
    }
    else if ( m_name == "coronal" )
    {
        mi = m_globalModel->index( (int)Fn::Global::CORONAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_globalModel->data( mi ).toInt() );
        }
        mi = m_globalModel->index( (int)Fn::Global::MAX_CORONAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_globalModel->data( mi ).toInt() - 1 );
        }
    }
    else if ( m_name == "axial" )
    {
        mi = m_globalModel->index( (int)Fn::Global::AXIAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setValue( m_globalModel->data( mi ).toInt() );
        }
        mi = m_globalModel->index( (int)Fn::Global::MAX_AXIAL, 0 );
        if ( mi.isValid() )
        {
            m_slider->setMax( m_globalModel->data( mi ).toInt() - 1 );
        }
    }
}

void DockNavGLWidget::update()
{
    m_glWidget->update();
}
