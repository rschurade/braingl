/*
 * datasetpropertywidget.cpp
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */
#include "datasetpropertywidget.h"

#include "../views/datasetpropertyview.h"

#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/sliderwitheditint.h"
#include "../widgets/controls/selectwithlabel.h"
#include "../widgets/controls/checkboxwithlabel.h"
#include "../widgets/controls/colormapwidget.h"

#include "../gl/colormapfunctions.h"
#include "../gl/glfunctions.h"

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/property.h"

#include <QtGui>

DatasetPropertyWidget::DatasetPropertyWidget( QString target, QWidget* parent ) :
    QTabWidget( parent ),
    m_target( target )
{
    m_propertyView = new DatasetPropertyView( this );
    m_propertyView->setModel( Models::d() );

    setTabPosition( QTabWidget::South );

    connect( m_propertyView, SIGNAL( selectedChanged() ), this, SLOT( updateWidgetVisibility() ) );
}

DatasetPropertyWidget::~DatasetPropertyWidget()
{
}

void DatasetPropertyWidget::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_propertyView->setSelectionModel( selectionModel );
}

void DatasetPropertyWidget::updateWidgetVisibility()
{
    // clear tabs
    while ( count() > 0 )
    {
        removeTab( 0 );
    }
    repaint();
    // get properties
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    PropertyGroup* props = ds->properties( m_target );
    QHash<QString, QVBoxLayout*>tabs;

    for ( int i = 0; i < props->size(); ++i )
    {
        // check tab
        QString tab = props->getNthProperty( i )->getPropertyTab();
        if ( tab == "none" )
        {
            continue;
        }
        //create tab if not exists
        if ( !tabs.contains( tab ) )
        {
            QVBoxLayout* layout = new QVBoxLayout();
            layout->setContentsMargins( 1, 1, 1, 1 );
            layout->setSpacing( 1 );

            QWidget* widget = new QWidget;
            widget->setLayout( layout );
            widget->setContentsMargins( 0, 0, 0, 0 );

            addTab( widget, tab );
            tabs[tab] = layout;
        }
        // insert property in tab
        if ( props->getNthProperty( i )->getName() == Fn::Prop2String::s( Fn::Property::COLORMAP ) )
        {
            SelectWithLabel* cmapSel = new SelectWithLabel( "colormap", 0 );
            for ( int k = 0; k < ColormapFunctions::size(); ++k )
            {
                cmapSel->insertItem( k, ColormapFunctions::get( k ).getName() );

            }
            int selectedCmap = ds->properties( m_target )->get( Fn::Property::COLORMAP ).toInt();
            cmapSel->setCurrentIndex( selectedCmap );
            tabs[tab]->addWidget( cmapSel );
            connect( cmapSel, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( colormapSelectionChanged( int) ) );

            float min = ds->properties( m_target )->get( Fn::Property::MIN ).toFloat();
            float max = ds->properties( m_target )->get( Fn::Property::MAX ).toFloat();
            ColormapWidget* cmapWidget = new ColormapWidget( size().width() - 14, min, max );
            tabs[tab]->addWidget( cmapWidget );

            cmapWidget->setMin( ds->properties( m_target )->get( Fn::Property::SELECTED_MIN ).toFloat() );
            cmapWidget->setMax( ds->properties( m_target )->get( Fn::Property::SELECTED_MAX ).toFloat() );
            cmapWidget->setLowerThreshold( ds->properties( m_target )->get( Fn::Property::LOWER_THRESHOLD ).toFloat() );
            cmapWidget->setUpperThreshold( ds->properties( m_target )->get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
            cmapWidget->setColormap( selectedCmap );
            connect( ds->properties( m_target )->getWidget( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMin( float ) ) );
            connect( ds->properties( m_target )->getWidget( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMax( float ) ) );
            connect( ds->properties( m_target )->getWidget( Fn::Property::LOWER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setLowerThreshold( float ) ) );
            connect( ds->properties( m_target )->getWidget( Fn::Property::UPPER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setUpperThreshold( float ) ) );
            connect( cmapSel, SIGNAL( currentIndexChanged( int, int) ), cmapWidget, SLOT( setColormap( int ) ) );
        }
        else
        {
            tabs[tab]->addWidget( props->getNthProperty( i )->getWidget() );
        }
    }

    QHashIterator<QString, QVBoxLayout*> ti( tabs );
    while ( ti.hasNext() )
    {
        ti.next();
        ti.value()->addStretch();
    }
}

void DatasetPropertyWidget::colormapSelectionChanged( int id )
{
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    ds->properties( m_target )->set( Fn::Property::COLORMAP, id );
    ds->properties( m_target )->slotPropChanged();

    GLFunctions::reloadShaders();
    Models::g()->submit();
}

void DatasetPropertyWidget::update()
{
    updateWidgetVisibility();
}
