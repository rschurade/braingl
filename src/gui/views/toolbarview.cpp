/*
 * toolbarview.cpp
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include "toolbarview.h"

ToolBarView::ToolBarView( QWidget* parent )
{
}

ToolBarView::~ToolBarView()
{
}

QRect ToolBarView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void ToolBarView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex ToolBarView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex ToolBarView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int ToolBarView::horizontalOffset() const
{
    return 0;
}

int ToolBarView::verticalOffset() const
{
    return 0;
}

bool ToolBarView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void ToolBarView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion ToolBarView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void ToolBarView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    if ( selected.indexes().size() > 0 )
    {
        int sel = -1;
        sel = selected.indexes().first().row();
        QModelIndex index = model()->index( sel, 12 );
        int type = model()->data( index, Qt::DisplayRole ).toInt();

        emit( sigSelectionChanged( type ) );
    }
}
