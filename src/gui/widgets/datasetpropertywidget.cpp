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

#include "../gl/colormapfunctions.h"

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/property.h"

#include <QtGui/QtGui>

DatasetPropertyWidget::DatasetPropertyWidget( QWidget* parent ) :
    QTabWidget( parent )
{
    m_propertyView = new DatasetPropertyView( this );

    m_layout1 = new QVBoxLayout();
    m_layout1->setContentsMargins( 1, 1, 1, 1 );
    m_layout1->setSpacing( 1 );
    m_layout1->addStretch();

    m_propWidget = new QWidget;
    m_propWidget->setLayout( m_layout1 );
    m_propWidget->setContentsMargins( 0, 0, 0, 0 );

    m_layout2 = new QVBoxLayout();
    m_layout2->setContentsMargins( 1, 1, 1, 1 );
    m_layout2->setSpacing( 1 );
    m_layout2->addStretch();

    m_colormapWidget = new QWidget;
    m_colormapWidget->setLayout( m_layout2 );
    m_colormapWidget->setContentsMargins( 0, 0, 0, 0 );

    setTabPosition( QTabWidget::South );

    addTab( m_propWidget, "general" );
    addTab( m_colormapWidget, "colormap" );

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

    for ( int i = 0; i < m_visibleWidgets1.size(); ++i )
    {
        m_visibleWidgets1[i]->hide();
        m_layout1->removeWidget( m_visibleWidgets1[i] );
    }
    m_layout1->removeItem( m_layout1->itemAt( 0 ) );
    m_visibleWidgets1.clear();

    for ( int i = 0; i < m_visibleWidgets2.size(); ++i )
    {
        m_visibleWidgets2[i]->hide();
        m_layout2->removeWidget( m_visibleWidgets2[i] );
    }
    m_layout2->removeItem( m_layout2->itemAt( 0 ) );
    m_visibleWidgets2.clear();

    repaint();

    for ( int i = 0; i < visible.size(); ++i )
    {
        switch ( visible[i] )
        {
            case Fn::Property::COLORMAP:
            {
                SelectWithLabel* cmapSel = new SelectWithLabel( "colormap", 0 );
                m_visibleWidgets2.push_back( cmapSel );

                for ( int k = 0; k < ColormapFunctions::size(); ++k )
                {
                    cmapSel->insertItem( k, ColormapFunctions::get( k ).getName() );

                }
                int selectedCmap = ds->properties()->get( Fn::Property::COLORMAP ).toInt();
                cmapSel->setCurrentIndex( selectedCmap );
                m_layout2->addWidget( cmapSel );
                connect( cmapSel, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( colormapSelectionChanged( int) ) );

                float min = ds->properties()->get( Fn::Property::MIN ).toFloat();
                float max = ds->properties()->get( Fn::Property::MAX ).toFloat();
                ColormapWidget* cmapWidget = new ColormapWidget( size().width() - 10, min, max );
                m_layout2->addWidget( cmapWidget );
                m_visibleWidgets2.push_back( cmapWidget );
                cmapWidget->setMin( ds->properties()->get( Fn::Property::SELECTED_MIN ).toFloat() );
                cmapWidget->setMax( ds->properties()->get( Fn::Property::SELECTED_MAX ).toFloat() );
                cmapWidget->setLowerThreshold( ds->properties()->get( Fn::Property::LOWER_THRESHOLD ).toFloat() );
                cmapWidget->setUpperThreshold( ds->properties()->get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
                cmapWidget->setColormap( selectedCmap );
                connect( ds->properties()->getWidget( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMin( float ) ) );
                connect( ds->properties()->getWidget( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMax( float ) ) );
                connect( ds->properties()->getWidget( Fn::Property::LOWER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setLowerThreshold( float ) ) );
                connect( ds->properties()->getWidget( Fn::Property::UPPER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setUpperThreshold( float ) ) );
                connect( cmapSel, SIGNAL( currentIndexChanged( int, int) ), cmapWidget, SLOT( setColormap( int ) ) );
                break;
            }
            case Fn::Property::RENDER_COLORMAP:
            case Fn::Property::COLORMAP_X:
            case Fn::Property::COLORMAP_Y:
            case Fn::Property::COLORMAP_DX:
            case Fn::Property::COLORMAP_DY:
            case Fn::Property::COLORMAP_TEXT_SIZE:
            {
                m_layout2->addWidget( ds->properties()->getWidget( visible[i] ) );
                ds->properties()->getWidget( visible[i] )->show();
                m_visibleWidgets2.push_back( ds->properties()->getWidget( visible[i] ) );
                break;
            }

            default:
            {
                m_layout1->addWidget( ds->properties()->getWidget( visible[i] ) );
                ds->properties()->getWidget( visible[i] )->show();
                m_visibleWidgets1.push_back( ds->properties()->getWidget( visible[i] ) );
                break;
            }
        }
    }

    m_layout1->addStretch();
    m_layout2->addStretch();
}

void DatasetPropertyWidget::colormapSelectionChanged( int id )
{
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    ds->properties()->set( Fn::Property::COLORMAP, id );
    ds->properties()->slotPropChanged();
}

void DatasetPropertyWidget::update()
{
    updateWidgetVisibility();
}
