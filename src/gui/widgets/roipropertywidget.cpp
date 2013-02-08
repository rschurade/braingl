/*
 * roipropertywidget.cpp
 *
 *  Created on: 03.02.2013
 *      Author: Ralph Schurade
 */
#include "roipropertywidget.h"

#include "../views/roipropertyview.h"

#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/sliderwitheditint.h"
#include "../widgets/controls/selectwithlabel.h"
#include "../widgets/controls/checkboxwithlabel.h"

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"
#include "../../data/properties/property.h"

#include <QtGui/QtGui>

ROIPropertyWidget::ROIPropertyWidget( QWidget* parent ) :
    QWidget( parent )
{
    m_propertyView = new ROIPropertyView( this );

    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 1, 1, 1, 1 );
    m_layout->setSpacing( 1 );
    m_layout->addStretch();
    setLayout( m_layout );

    setContentsMargins( 0, 0, 0, 0 );

    connect( m_propertyView, SIGNAL( selectedChanged() ), this, SLOT( updateWidgetVisibility() ) );
    connect( m_propertyView, SIGNAL( nothingSelected() ), this, SLOT( clearWidget() ) );
}

ROIPropertyWidget::~ROIPropertyWidget()
{
}

void ROIPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );
}

void ROIPropertyWidget::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_propertyView->setSelectionModel( selectionModel );
}

void ROIPropertyWidget::updateWidgetVisibility()
{
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::ROI::POINTER );

    ROI* roi = VPtr<ROI>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    QList<Fn::ROI>visible = roi->properties()->getVisible();

    for ( int i = 0; i < m_visibleWidgets.size(); ++i )
    {
        m_visibleWidgets[i]->hide();
        m_layout->removeWidget( m_visibleWidgets[i] );
    }
    m_layout->removeItem( m_layout->itemAt( 0 ) );
    m_visibleWidgets.clear();
    repaint();

    for ( int i = 0; i < visible.size(); ++i )
    {
        m_layout->addWidget( roi->properties()->getWidget( visible[i] ) );
        roi->properties()->getWidget( visible[i] )->show();
        m_visibleWidgets.push_back( roi->properties()->getWidget( visible[i] ) );
    }
    m_layout->addStretch();
}

void ROIPropertyWidget::clearWidget()
{
    for ( int i = 0; i < m_visibleWidgets.size(); ++i )
    {
        m_visibleWidgets[i]->hide();
        m_layout->removeWidget( m_visibleWidgets[i] );
    }
    m_layout->removeItem( m_layout->itemAt( 0 ) );
    m_visibleWidgets.clear();
    repaint();

    m_layout->addStretch();
}
