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
#include "../widgets/controls/checkbox.h"
#include "../widgets/controls/colormapwidget.h"

#include "../gl/colormapfunctions.h"
#include "../gl/glfunctions.h"

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/property.h"

#include <QtWidgets>

DatasetPropertyWidget::DatasetPropertyWidget( QString target, QWidget* parent ) :
    QTabWidget( parent ),
    m_target( target ),
    m_buildingTabs( false )
{
    m_propertyView = new DatasetPropertyView( this );
    m_propertyView->setModel( Models::d() );

    setTabPosition( QTabWidget::North );

    connect( m_propertyView, SIGNAL( selectedChanged() ), this, SLOT( updateWidgetVisibility() ) );
    connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( slotTabChanged( int ) ) );
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
    m_buildingTabs = true;
    // clear tabs
    while ( count() > 0 )
    {
        removeTab( 0 );
    }
    repaint();
    // get properties
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::D_DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    if ( !ds )
    {
        return;
    }
    PropertyGroup* props = &ds->properties( m_target );
    m_tabs.clear();

    for ( int i = 0; i < props->size(); ++i )
    {
        // check tab
        QString tab = props->getNthProperty( i )->getPropertyTab();
        if ( tab == "none" )
        {
            continue;
        }
        //create tab if not exists
        if ( !m_tabs.contains( tab ) )
        {
            QVBoxLayout* layout = new QVBoxLayout();
            layout->setContentsMargins( 1, 1, 1, 1 );
            layout->setSpacing( 1 );

            QWidget* widget = new QWidget;
            widget->setLayout( layout );
            widget->setContentsMargins( 0, 0, 0, 0 );

            QScrollArea* scrollArea = new QScrollArea( this );
            scrollArea->setWidgetResizable( true );
            scrollArea->setWidget( widget );

            addTab( scrollArea, tab );
            m_tabs[tab] = layout;
        }
        // insert property in tab
        if ( props->getNthProperty( i )->getName() == Fn::Prop2String::s( Fn::Property::D_COLORMAP ) )
        {
            SelectWithLabel* cmapSel = new SelectWithLabel( "colormap", 0 );
            for ( int k = 0; k < ColormapFunctions::size(); ++k )
            {
                cmapSel->insertItem( k, ColormapFunctions::get( k ).getName() );

            }
            PropertyGroup* props = &ds->properties( m_target );

            int selectedCmap = props->get( Fn::Property::D_COLORMAP ).toInt();
            cmapSel->setCurrentIndex( selectedCmap );
            m_tabs[tab]->addWidget( cmapSel );
            connect( cmapSel, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( colormapSelectionChanged( int) ) );

            float min = props->get( Fn::Property::D_MIN ).toFloat();
            float max = props->get( Fn::Property::D_MAX ).toFloat();
            ColormapWidget* cmapWidget = new ColormapWidget( size().width() - 14, min, max );
            m_tabs[tab]->addWidget( cmapWidget );

            cmapWidget->setMin( props->get( Fn::Property::D_SELECTED_MIN ).toFloat() );
            cmapWidget->setMax( props->get( Fn::Property::D_SELECTED_MAX ).toFloat() );
            cmapWidget->setLowerThreshold( props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() );
            cmapWidget->setUpperThreshold( props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() );
            cmapWidget->setColormap( selectedCmap );

            if ( props->getWidget( Fn::Property::D_SELECTED_MIN ) )
            {
                connect( props->getWidget( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMin( float ) ) );
                connect( props->getWidget( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setMax( float ) ) );
                connect( props->getWidget( Fn::Property::D_LOWER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setLowerThreshold( float ) ) );
                connect( props->getWidget( Fn::Property::D_UPPER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setUpperThreshold( float ) ) );
            }
            connect( cmapSel, SIGNAL( currentIndexChanged( int, int) ), cmapWidget, SLOT( setColormap( int ) ) );
        }
        else
        {
            m_tabs[tab]->addWidget( props->getNthProperty( i )->getWidget() );
        }
    }

    QHashIterator<QString, QVBoxLayout*> ti( m_tabs );
    while ( ti.hasNext() )
    {
        ti.next();
        ti.value()->addStretch();
    }

    for ( int i = 0; i < count(); ++i )
    {
        if ( m_lastUsedTab == tabText( i ) )
        {
            setCurrentIndex( i );
            break;
        }
    }
    m_buildingTabs = false;
}

void DatasetPropertyWidget::colormapSelectionChanged( int id )
{
    QModelIndex index = m_propertyView->getSelectedIndex( (int)Fn::Property::D_DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( m_propertyView->model()->data( index, Qt::DisplayRole ) );
    ds->properties( m_target ).set( Fn::Property::D_COLORMAP, id );
    ds->properties( m_target ).slotPropChanged();

    GLFunctions::reloadShaders();
    Models::g()->submit();
}

void DatasetPropertyWidget::update()
{
    updateWidgetVisibility();
}

void DatasetPropertyWidget::slotTabChanged( int tab )
{
    if( m_buildingTabs )
    {
        return;
    }
    m_lastUsedTab = tabText( tab );
}
