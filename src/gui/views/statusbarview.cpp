/*
 * statusbarview.cpp
 *
 *  Created on: Nov 13, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "../../data/enums.h"
#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"

#include "statusbarview.h"

StatusBarView::StatusBarView() :
    m_selected( 0 ),
    m_globalInfo( "" ),
    m_datasetInfo( "" ),
    m_x( 0 ),
    m_y( 0 ),
    m_z( 0 )
{
}

StatusBarView::~StatusBarView()
{
}

QRect StatusBarView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void StatusBarView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex StatusBarView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex StatusBarView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int StatusBarView::horizontalOffset() const
{
    return 0;
}

int StatusBarView::verticalOffset() const
{
    return 0;
}

bool StatusBarView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void StatusBarView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion StatusBarView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void StatusBarView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    if ( selected.indexes().size() > 0 )
    {
        m_selected = selected.indexes().first().row();
        Dataset* ds = VPtr<Dataset>::asPtr( model()->data( model()->index( m_selected, FNPROP_DATASET_POINTER ), Qt::DisplayRole ) );
        m_datasetInfo = ds->getValueAsString( m_x, m_y, m_z );

        emit( sigStatusChanged( m_globalInfo + " " + m_datasetInfo ) );
    }
    else
    {
        m_selected = -1;
    }
}

void StatusBarView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    if ( m_selected == -1 )
    {
        return;
    }
    m_globalInfo = "[";

    QModelIndex mi;
    mi = model()->index( 0, FNGLOBAL_SAGITTAL );
    if ( mi.isValid() )
    {
        m_x = model()->data( mi, Qt::UserRole ).toInt();
        m_globalInfo +=  QString::number( m_x );
        m_globalInfo += ",";
    }

    mi = model()->index( 0, FNGLOBAL_CORONAL );
    if ( mi.isValid() )
    {
        m_y = model()->data( mi, Qt::UserRole ).toInt();
        m_globalInfo += QString::number( m_y );
        m_globalInfo += ",";
    }

    mi = model()->index( 0, FNGLOBAL_AXIAL );
    if ( mi.isValid() )
    {
        m_z = model()->data( mi, Qt::UserRole ).toInt();
        m_globalInfo += QString::number( m_z );
        m_globalInfo += "]";
    }

    Dataset* ds = VPtr<Dataset>::asPtr( model()->data( model()->index( m_selected, FNPROP_DATASET_POINTER ), Qt::DisplayRole ) );
    if ( ds )
    {
        m_datasetInfo = ds->getValueAsString( m_x, m_y, m_z );
    }

    emit( sigStatusChanged( m_globalInfo + " " + m_datasetInfo ) );
}
