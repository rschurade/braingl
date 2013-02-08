/*
 * datasetpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph Schurade
 */
#include "datasetpropertywidget.h"

#include "../views/datasetpropertyview.h"

#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/sliderwitheditint.h"
#include "../widgets/controls/selectwithlabel.h"
#include "../widgets/controls/checkboxwithlabel.h"
#include "../widgets/controls/colormapwidget.h"

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/property.h"

#include <QtGui/QtGui>

DatasetPropertyWidget::DatasetPropertyWidget( QWidget* parent ) :
    QWidget( parent )
{
    m_propertyView = new DatasetPropertyView( this );

    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 1, 1, 1, 1 );
    m_layout->setSpacing( 1 );
    m_layout->addStretch();
    setLayout( m_layout );

    setContentsMargins( 0, 0, 0, 0 );

    connect( m_propertyView, SIGNAL( selectedChanged() ), this, SLOT( updateWidgetVisibility() ) );
}

DatasetPropertyWidget::~DatasetPropertyWidget()
{
}

void DatasetPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );
}

void DatasetPropertyWidget::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_propertyView->setSelectionModel( selectionModel );
}

void DatasetPropertyWidget::updateWidgetVisibility()
{
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    QList<Fn::Property>visible = ds->properties()->getVisible();

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
        m_layout->addWidget( ds->properties()->getWidget( visible[i] ) );
        ds->properties()->getWidget( visible[i] )->show();
        m_visibleWidgets.push_back( ds->properties()->getWidget( visible[i] ) );
        if ( visible[i] == Fn::Property::COLORMAP )
        {
            ColormapWidget* cmapWidget = new ColormapWidget( size().width() - 6 );
            m_layout->addWidget( cmapWidget );
            m_visibleWidgets.push_back( cmapWidget );
            cmapWidget->setMin( ds->properties()->get( Fn::Property::MIN ).toFloat() );
            cmapWidget->setMax( ds->properties()->get( Fn::Property::MAX ).toFloat() );
            cmapWidget->setLowerThreshold( ds->properties()->get( Fn::Property::LOWER_THRESHOLD ).toFloat() );
            cmapWidget->setUpperThreshold( ds->properties()->get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
            cmapWidget->setColormap( ds->properties()->get( Fn::Property::COLORMAP ).toInt() );
            connect( ds->properties()->getWidget( Fn::Property::MIN ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMin( float ) ) );
            connect( ds->properties()->getWidget( Fn::Property::MAX ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMax( float ) ) );
            connect( ds->properties()->getWidget( Fn::Property::LOWER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setLowerThreshold( float ) ) );
            connect( ds->properties()->getWidget( Fn::Property::UPPER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setUpperThreshold( float ) ) );
            connect( ds->properties()->getWidget( Fn::Property::COLORMAP ), SIGNAL( valueChanged( int, int) ), cmapWidget, SLOT( setColormap( int ) ) );
        }
    }
    m_layout->addStretch();
}
