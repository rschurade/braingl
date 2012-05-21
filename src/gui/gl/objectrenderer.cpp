/*
 * objectrenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#include "objectrenderer.h"

ObjectRenderer::ObjectRenderer() :
	QAbstractItemView()
{
}

ObjectRenderer::~ObjectRenderer()
{
}

QRect ObjectRenderer::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void ObjectRenderer::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex ObjectRenderer::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex ObjectRenderer::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int ObjectRenderer::horizontalOffset() const
{
    return 0;
}

int ObjectRenderer::verticalOffset() const
{
    return 0;
}

bool ObjectRenderer::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void ObjectRenderer::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion ObjectRenderer::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}
