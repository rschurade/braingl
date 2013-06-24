/*
 * globalpropertywidget.cpp
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */
#include "globalpropertywidget.h"
#include "../views/globalpropertyview.h"

#include "../widgets/controls/sliderwitheditint.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/properties/propertygroup.h"

#include <QtGui>

GlobalPropertyWidget::GlobalPropertyWidget( QWidget* parent ) :
    QTabWidget( parent )
{
    m_propertyView = new GlobalPropertyView( this );
    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 1, 1, 1, 1 );
    m_layout->setSpacing( 1 );
    setLayout( m_layout );

    setContentsMargins( 0, 0, 0, 0 );
}

GlobalPropertyWidget::~GlobalPropertyWidget()
{
}

void GlobalPropertyWidget::setModel( QAbstractItemModel* model )
{
    m_propertyView->setModel( model );
    m_propertyView->init();

    PropertyGroup* props = VPtr<PropertyGroup>::asPtr( model->data( model->index( (int)Fn::Property::G_OBJECT, 0 ) ) );

    // clear tabs
    while ( count() > 0 )
    {
        removeTab( 0 );
    }
    repaint();
    // get properties

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
        tabs[tab]->addWidget( props->getNthProperty( i )->getWidget() );
    }

    QHashIterator<QString, QVBoxLayout*> ti( tabs );
    while ( ti.hasNext() )
    {
        ti.next();
        ti.value()->addStretch();
    }

}

void GlobalPropertyWidget::sliderChanged( int value, int id )
{
    m_propertyView->model()->setData( m_propertyView->model()->index( id, 0 ), value );
}

void GlobalPropertyWidget::dataChanged()
{
}

