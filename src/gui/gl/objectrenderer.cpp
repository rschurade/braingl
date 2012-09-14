/*
 * objectrenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#include <QtCore/QDebug>
#include "objectrenderer.h"

ObjectRenderer::ObjectRenderer() :
	QAbstractItemView(),
	m_previousSettings( "" )
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

QString ObjectRenderer::createSettingsString( int x, int y, int z,
                                              int lod, int orient,
                                              int lx, int ux, int ly, int uy, int lz, int uz,
                                              bool scaling, int bValue )
{
    QString result("");
    result += QString::number( x );
    result += QString::number( y );
    result += QString::number( z );
    result += QString::number( lod );
    result += QString::number( orient );
    result += QString::number( lx );
    result += QString::number( ux );
    result += QString::number( ly );
    result += QString::number( uy );
    result += QString::number( lz );
    result += QString::number( uz );
    result += QString::number( scaling );
    result += QString::number( bValue );
    return result;
}

int ObjectRenderer::getMaxLod( int orient, int lx, int ux, int ly, int uy, int lz, int uz )
{
    int maxO = 5;
    int x = ( ux - lx);
    int y = ( uy - ly);
    int z = ( uz - lz);
    int count = 0;
    switch( orient )
    {
        case 1:
            count = x*y;
            break;
        case 2:
            count = x*z;
            break;
        case 3:
            count = z*y;
            break;
    }
    if ( count > 2000 )
    {
        maxO = 2;
    }
    else if ( count > 1000 )
    {
        maxO = 3;
    }
    else if ( count > 400 )
    {
        maxO = 4;
    }
    return maxO;
}
