/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/sliderwitheditint.h"
#include "../widgets/controls/selectwithlabel.h"
#include "../widgets/controls/checkboxwithlabel.h"
#include "../widgets/controls/colormapwidget.h"
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
    m_layout->setContentsMargins( 1, 0, 1, 0 );
    m_layout->setSpacing( 1 );
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

    if ( m_selected.indexes().size() > 0 )
    {
        updateWidgetVisibility();
    }
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
        if ( visible[i] == FNPROP_COLORMAP )
        {
            ColormapWidget* cmapWidget = new ColormapWidget( m_widget->size().width() );
            m_layout->addWidget( cmapWidget );
            m_visibleWidgets.push_back( cmapWidget );
            cmapWidget->setLowerThreshold( ds->properties()->get( FNPROP_LOWER_THRESHOLD ).toFloat() );
            cmapWidget->setUpperThreshold( ds->properties()->get( FNPROP_UPPER_THRESHOLD ).toFloat() );
            cmapWidget->setColormap( ds->properties()->get( FNPROP_COLORMAP ).toInt() );
            connect( ds->properties()->getWidget( FNPROP_LOWER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setLowerThreshold( float ) ) );
            connect( ds->properties()->getWidget( FNPROP_UPPER_THRESHOLD ), SIGNAL( valueChanged( float, int) ), cmapWidget, SLOT( setUpperThreshold( float ) ) );
            connect( ds->properties()->getWidget( FNPROP_COLORMAP ), SIGNAL( valueChanged( int, int) ), cmapWidget, SLOT( setColormap( int ) ) );
        }
    }
    m_layout->addStretch();
}
