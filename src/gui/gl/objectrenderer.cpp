/*
 * objectrenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph Schurade
 */
#include "objectrenderer.h"

#include "../../data/enums.h"

#include <QtCore/QDebug>

ObjectRenderer::ObjectRenderer() :
	QAbstractItemView(),
	m_previousSettings( "" ),
	m_visibleArea( 6 )
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

QString ObjectRenderer::createSettingsString( int x, int y, int z, int orient,
                                              int lx, int ux, int ly, int uy, int lz, int uz,
                                              bool scaling, int bValue, int lod, float offset )
{
    QString result("");
    result += QString::number( x );
    result += QString::number( y );
    result += QString::number( z );
    result += QString::number( orient );
    result += QString::number( lx );
    result += QString::number( ux );
    result += QString::number( ly );
    result += QString::number( uy );
    result += QString::number( lz );
    result += QString::number( uz );
    result += QString::number( scaling );
    result += QString::number( bValue );
    result += QString::number( lod );
    result += QString::number( offset );
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
    if ( count > 800 )
    {
        maxO = 1;
    }
    else if ( count > 400 )
    {
        maxO = 2;
    }
    else if ( count > 200 )
    {
        maxO = 3;
    }
    else if ( count > 60 )
    {
        maxO = 4;
    }
    return maxO;
}

void ObjectRenderer::calcBounds( int nx, int ny, int nz, float dx, float dy, float dz, int orient )
{
    m_zoom = model()->data( model()->index( (int)Fn::Global::ZOOM, 0 ) ).toFloat();
    m_moveX = model()->data( model()->index( (int)Fn::Global::MOVEX, 0 ) ).toFloat();
    m_moveY = model()->data( model()->index( (int)Fn::Global::MOVEY, 0 ) ).toFloat();
    m_bbX = model()->data( model()->index( (int)Fn::Global::BBX, 0 ) ).toFloat();
    m_bbY = model()->data( model()->index( (int)Fn::Global::BBY, 0 ) ).toFloat();


    switch( orient )
    {
        case 1:
        {
            int xVisible = qMin( nx, (int)( ( m_bbX/dx ) / m_zoom ) );
            int yVisible = qMin( ny, (int)( ( m_bbY/dy ) / m_zoom ) );
            m_visibleArea[0] = qMax( 0,  ( ( nx - xVisible ) / 2 ) - 1             + (int)( m_moveX  / ( 3.4 * dx ) ) );
            m_visibleArea[1] = qMin( nx, ( ( nx - xVisible ) / 2 ) + xVisible + 1  + (int)( m_moveX  / ( 3.4 * dx ) ) );
            m_visibleArea[2] = qMax( 0,  ( ( ny - yVisible ) / 2 ) - 1             - (int)( m_moveY  / ( 3.4 * dy ) ) );
            m_visibleArea[3] = qMin( ny, ( ( ny - yVisible ) / 2 ) + yVisible + 1  - (int)( m_moveY  / ( 3.4 * dy ) ) );

            //qDebug() << m_moveX << m_zoom << m_moveX  / dx << m_moveX  / ( 3.4 * dx * m_zoom)   ;

            if ( xVisible >= nx-1 )
            {
                m_visibleArea[0] = 0;
                m_visibleArea[1] = nx-1;
            }

            if ( yVisible >= ny-1 )
            {
                m_visibleArea[2] = 0;
                m_visibleArea[3] = ny-1;
            }
            //qDebug() << m_visibleArea[0] << m_visibleArea[1] << m_visibleArea[2] << m_visibleArea[3];
            break;
        }
        case 2:
        {
            int xVisible = qMin( nx, (int)( ( m_bbX/dx ) / m_zoom ) );
            int zVisible = qMin( nz, (int)( ( m_bbY/dz ) / m_zoom ) );
            m_visibleArea[0] = qMax( 0,  ( ( nx - xVisible ) / 2 ) - 1            + (int)( m_moveX  / ( 3.4 * dx ) ) );
            m_visibleArea[1] = qMin( nx, ( ( nx - xVisible ) / 2 ) + xVisible + 1 + (int)( m_moveX  / ( 3.4 * dx ) ) );
            m_visibleArea[4] = qMax( 0,  ( ( nz - zVisible ) / 2 ) - 1            - (int)( m_moveY  / ( 3.4 * dz ) ) );
            m_visibleArea[5] = qMin( nz, ( ( nz - zVisible ) / 2 ) + zVisible + 1 - (int)( m_moveY  / ( 3.4 * dz ) ) );

            if ( xVisible >= nx-1 )
            {
                m_visibleArea[0] = 0;
                m_visibleArea[1] = nx-1;
            }

            if ( zVisible >= nz-1 )
            {
                m_visibleArea[4] = 0;
                m_visibleArea[5] = nz-1;
            }
            break;
        }
        case 3:
        {
            int yVisible = qMin( ny, (int)( ( m_bbX/dy ) / m_zoom ) );
            int zVisible = qMin( nz, (int)( ( m_bbY/dz ) / m_zoom ) );
            m_visibleArea[2] = qMax( 0,  ( ( ny - yVisible ) / 2 ) - 1            - (int)( m_moveX  / ( 3.4 * dy ) ) );
            m_visibleArea[3] = qMin( ny, ( ( ny - yVisible ) / 2 ) + yVisible + 1 - (int)( m_moveX  / ( 3.4 * dy ) ) );
            m_visibleArea[4] = qMax( 0,  ( ( nz - zVisible ) / 2 ) - 1            - (int)( m_moveY  / ( 3.4 * dz ) ) );
            m_visibleArea[5] = qMin( nz, ( ( nz - zVisible ) / 2 ) + zVisible + 1 - (int)( m_moveY  / ( 3.4 * dz ) ) );

            if ( yVisible >= ny-1 )
            {
                m_visibleArea[2] = 0;
                m_visibleArea[3] = ny-1;
            }

            if ( zVisible >= nz-1 )
            {
                m_visibleArea[4] = 0;
                m_visibleArea[5] = nz-1;
            }
            break;
        }
    }
}
