/*
 * sroitreeview.cpp
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#include "roitreeview.h"

#include "../../data/roiarea.h"
#include "../../data/roibox.h"
#include "../../data/vptr.h"

#include "../gl/glfunctions.h"

#include <QDebug>

ROITreeView::ROITreeView( QWidget* parent ) :
    QTreeView( parent )
{
}

ROITreeView::~ROITreeView()
{
}

void ROITreeView::addBox()
{
    GLFunctions::roi = new ROIBox();

    if ( selectionModel()->hasSelection() )
    {
        model()->insertRows( 0, 0, selectionModel()->selectedIndexes().first() );
        expand( selectionModel()->selectedRows().at( 0 ) );
    }
    else
    {
        // nothing selected, inserting top box
        model()->insertRows( 0, 0, QModelIndex() );
    }
}

void ROITreeView::addROIArea( ROIArea* roi )
{
    GLFunctions::roi = roi;

    if ( selectionModel()->hasSelection() )
    {
        model()->insertRows( 0, 0, selectionModel()->selectedIndexes().first() );
        expand( selectionModel()->selectedRows().at( 0 ) );
    }
    else
    {
        // nothing selected, inserting top box
        model()->insertRows( 0, 0, QModelIndex() );
    }
}


void ROITreeView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt( event->pos() );
    bool selected = selectionModel()->isSelected( indexAt( event->pos() ) );
    QTreeView::mousePressEvent( event );
    if ( ( item.row() == -1 && item.column() == -1 ) || selected )
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex( index, QItemSelectionModel::Select );
        emit nothingSelected();
    }

    repaint();
}

void ROITreeView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    emit itemSelectionChanged( selected );
}

void ROITreeView::rowsInserted (const QModelIndex &parent, int start, int end )
{
    //qDebug() << parent << start << end;
    if ( selectionModel()->hasSelection() )
    {
        expand( selectionModel()->selectedIndexes().first() );
    }
    selectionModel()->clear();
    if ( parent.row() == -1 )
    {
        selectionModel()->select( model()->index( start, 0, parent ), QItemSelectionModel::Select );
    }
    else
    {
        selectionModel()->select( model()->index( start-1, 0, parent ), QItemSelectionModel::Select );
    }
    QTreeView::rowsInserted( parent, start, end );
}
