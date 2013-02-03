/*
 * selectionboxtreeview.cpp
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#include "selectionboxtreeview.h"

SelectionBoxTreeView::SelectionBoxTreeView( QWidget* parent ) :
    QTreeView( parent )
{
}

SelectionBoxTreeView::~SelectionBoxTreeView()
{
}

void SelectionBoxTreeView::addBox()
{
    if ( selectionModel()->selectedRows().size() > 0 )
    {
        model()->insertRows( 0, 1, selectionModel()->selectedRows().at( 0 ) );
        expand( selectionModel()->selectedRows().at( 0 ) );
    }
    else
    {
        model()->insertRows( 0, 1, QModelIndex() );
    }
}

void SelectionBoxTreeView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt( event->pos() );
    bool selected = selectionModel()->isSelected( indexAt( event->pos() ) );
    QTreeView::mousePressEvent( event );
    if ( ( item.row() == -1 && item.column() == -1 ) || selected )
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex( index, QItemSelectionModel::Select );
    }
}
