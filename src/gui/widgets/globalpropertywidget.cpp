/*
 * globalpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph Schurade
 */
#include "globalpropertywidget.h"
#include "../views/globalpropertyview.h"

#include "../widgets/controls/sliderwitheditint.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/properties/globalpropertygroup.h"

#include <QtGui/QtGui>

GlobalPropertyWidget::GlobalPropertyWidget( QWidget* parent ) :
    QWidget( parent )
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

    GlobalPropertyGroup* props = VPtr<GlobalPropertyGroup>::asPtr( model->data( model->index( (int)Fn::Global::OBJECT, 0 ) ) );
    QList<Fn::Global>visible = props->getVisible();

    for ( int i = 0; i < visible.size(); ++i )
    {
        m_layout->addWidget( props->getWidget( visible[i] ) );
        props->getWidget( visible[i] )->show();
    }
    m_layout->addStretch();
}

void GlobalPropertyWidget::sliderChanged( int value, int id )
{
    m_propertyView->model()->setData( m_propertyView->model()->index( id, 0 ), value );
}

void GlobalPropertyWidget::dataChanged()
{
}

