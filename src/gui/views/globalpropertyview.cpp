/*
 * globalpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include "globalpropertyview.h"

GlobalPropertyView::GlobalPropertyView( QWidget* parent )
{
}

GlobalPropertyView::~GlobalPropertyView()
{
}

void GlobalPropertyView::init()
{
}

QRect GlobalPropertyView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void GlobalPropertyView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex GlobalPropertyView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex GlobalPropertyView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int GlobalPropertyView::horizontalOffset() const
{
    return 0;
}

int GlobalPropertyView::verticalOffset() const
{
    return 0;
}

bool GlobalPropertyView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void GlobalPropertyView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion GlobalPropertyView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void GlobalPropertyView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    emit gotDataChanged();
}
