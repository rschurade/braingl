/*
 * roipropertyview.cpp
 *
 *  Created on: 03.02.2012
 *      Author: Ralph
 */
#include "roipropertyview.h"


ROIPropertyView::ROIPropertyView( QWidget* parent ) :
    m_selected( QItemSelection() )
{
}

ROIPropertyView::~ROIPropertyView()
{
}

QRect ROIPropertyView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void ROIPropertyView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex ROIPropertyView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex ROIPropertyView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int ROIPropertyView::horizontalOffset() const
{
    return 0;
}

int ROIPropertyView::verticalOffset() const
{
    return 0;
}

bool ROIPropertyView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void ROIPropertyView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion ROIPropertyView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

QModelIndex ROIPropertyView::getSelectedIndex( int column )
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

void ROIPropertyView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    m_selected = selected;

    if ( m_selected.indexes().size() > 0 )
    {
        emit( selectedChanged() );
    }
}

void ROIPropertyView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    if ( m_selected.indexes().size() > 0 )
    {
       // emit( selectedChanged() );
    }
}
