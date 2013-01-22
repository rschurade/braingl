/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../widgets/sliderwithedit.h"
#include "../widgets/sliderwitheditint.h"
#include "../widgets/selectwithlabel.h"
#include "../widgets/checkboxwithlabel.h"
#include "../../data/enums.h"

#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"

#include "../../data/properties/property.h"

#include "datasetpropertyview.h"


DatasetPropertyView::DatasetPropertyView( QWidget* parent ) :
    m_selected( QItemSelection() )
{
    m_widget = new QWidget( this );
    m_layout = new QVBoxLayout();
    m_layout->addStretch();
    m_widget->setLayout( m_layout );
}

DatasetPropertyView::~DatasetPropertyView()
{
}

QWidget* DatasetPropertyView::getWidget()
{
    return m_widget;
}

QRect DatasetPropertyView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void DatasetPropertyView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex DatasetPropertyView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex DatasetPropertyView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int DatasetPropertyView::horizontalOffset() const
{
    return 0;
}

int DatasetPropertyView::verticalOffset() const
{
    return 0;
}

bool DatasetPropertyView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void DatasetPropertyView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion DatasetPropertyView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

QModelIndex DatasetPropertyView::getSelectedIndex( int column )
{
    if ( m_selected.indexes().size() > 0 )
    {
        int sel = -1;
        sel = m_selected.indexes().first().row();
        QModelIndex index = model()->index( sel, column );
        return index;
    }
    return QModelIndex();
}

void DatasetPropertyView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    m_selected = selected;

    updateWidgetVisibility();


}

void DatasetPropertyView::updateWidgetVisibility()
{
    QModelIndex index = getSelectedIndex( FNPROP_DATASET_POINTER );
    Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
    QList<FN_PROPERTY>visible = ds->properties()->getVisible();

    for ( int i = 0; i < m_visibleWidgets.size(); ++i )
    {
        m_visibleWidgets[i]->hide();
        m_layout->removeWidget( m_visibleWidgets[i] );
    }
    m_layout->removeItem( m_layout->itemAt( 0 ) );
    m_visibleWidgets.clear();
    m_widget->repaint();
    for ( int i = 0; i < visible.size(); ++i )
    {
        m_layout->addWidget( ds->properties()->getWidget( visible[i] ) );
        ds->properties()->getWidget( visible[i] )->show();
        m_visibleWidgets.push_back( ds->properties()->getWidget( visible[i] ) );
    }
    m_layout->addStretch();
}
