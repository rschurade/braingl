/*
 * globalpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include "globalpropertywidget.h"
#include "../views/globalpropertyview.h"

#include "../widgets/controls/sliderwitheditint.h"
#include "../../data/enums.h"

#include <QtGui/QtGui>

GlobalPropertyWidget::GlobalPropertyWidget( QWidget* parent ) :
    QWidget( parent )
{
    m_propertyView = new GlobalPropertyView( this );
    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 1, 0, 1, 0 );
    m_layout->setSpacing( 1 );

    m_sagittalSlider = new SliderWithEditInt( "sagittal", FNGLOBAL_SAGITTAL );
    m_coronalSlider = new SliderWithEditInt( "coronal", FNGLOBAL_CORONAL );
    m_axialSlider = new SliderWithEditInt( "axial", FNGLOBAL_AXIAL );

    m_layout->addWidget( m_sagittalSlider );
    m_layout->addWidget( m_coronalSlider );
    m_layout->addWidget( m_axialSlider );

    m_layout->addStretch( 0 );
    setLayout( m_layout );

    connect( m_sagittalSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
    connect( m_coronalSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
    connect( m_axialSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
    connect( m_propertyView, SIGNAL( gotDataChanged() ), this, SLOT( dataChanged() ) );
}

GlobalPropertyWidget::~GlobalPropertyWidget()
{
}

void GlobalPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );
    m_propertyView->init();
}

void GlobalPropertyWidget::sliderChanged( int value, int id )
{
    m_propertyView->model()->setData( m_propertyView->model()->index( id, 0 ), value );
}

void GlobalPropertyWidget::dataChanged()
{
    QModelIndex mi;
    mi = m_propertyView->model()->index( FNGLOBAL_SAGITTAL, 0 );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setValue( m_propertyView->model()->data( mi ).toInt() );
    }
    mi = m_propertyView->model()->index( FNGLOBAL_MAX_SAGITTAL, 0 );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setMax( m_propertyView->model()->data( mi ).toInt() - 1 );
    }

    mi = m_propertyView->model()->index( FNGLOBAL_CORONAL, 0 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setValue( m_propertyView->model()->data( mi ).toInt() );
    }
    mi = m_propertyView->model()->index( FNGLOBAL_MAX_CORONAL, 0 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setMax( m_propertyView->model()->data( mi ).toInt() - 1 );
    }

    mi = m_propertyView->model()->index( FNGLOBAL_AXIAL, 0 );
    if ( mi.isValid() )
    {
        m_axialSlider->setValue( m_propertyView->model()->data( mi ).toInt() );
    }
    mi = m_propertyView->model()->index( FNGLOBAL_MAX_AXIAL, 0 );
    if ( mi.isValid() )
    {
        m_axialSlider->setMax( m_propertyView->model()->data( mi ).toInt() - 1 );
    }
}

