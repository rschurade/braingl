/*
 * toolbarview.cpp
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/dwialgos.h"
#include "../../data/scalaralgos.h"
#include "../../data/fiberalgos.h"
#include "../../data/datasets/dataset.h"

#include "toolbarview.h"

ToolBarView::ToolBarView( QWidget* parent ) :
    m_selected( -1 )
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
        m_selected = selected.indexes().first().row();
        QModelIndex index = model()->index( m_selected, FNPROP_TYPE );
        int type = model()->data( index, Qt::DisplayRole ).toInt();

        emit( sigSelectionChanged( type ) );
    }
}

int ToolBarView::getSelected()
{
    return m_selected;
}

void ToolBarView::activateAlgo( FN_ALGO algo )
{
    QModelIndex index = model()->index( m_selected, FNPROP_DATASET_POINTER );
    QList<Dataset*>l;
    switch ( algo )
    {
        case FNALGO_QBALL4:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::qBallSharp( ds, 4 );
            }
            break;
        }
        case FNALGO_QBALL6:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::qBallSharp( ds, 6 );
            }
            break;
        }
        case FNALGO_QBALL8:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::qBallSharp( ds, 8 );
            }
            break;
        }
        case FNALGO_TENSORFIT:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::tensorFit( ds );
            }
            break;
        }
        case FNALGO_FA:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::calcFAFromDWI( ds );
            }
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
            {
                l = DWIAlgos::calcFAFromTensor( ds );
            }
            break;
        }
        case FNALGO_EV:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
            {
                l = DWIAlgos::calcEVFromDWI( ds );
            }
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
            {
                l = DWIAlgos::calcEVFromTensor( ds );
            }
            break;
        }
        case FNALGO_BINGHAM:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SH )
            {
                l = DWIAlgos::fitBingham( ds );
            }
            break;
        }
        case FNALGO_TENSOR_TRACK:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
            {
                l = DWIAlgos::tensorTrack( ds );
            }
            break;
        }
        case FNALGO_ISOSURFACE:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SCALAR )
            {
                l = ScalarAlgos::isoSurface( ds );
            }
            break;
        }
        case FNALGO_FIBER_THINNING:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_FIBERS )
            {
                l = FiberAlgos::thinOut( ds );
            }
            break;
        }
        case FNALGO_BINGHAM_2_TENSOR:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_BINGHAM )
            {
                l = DWIAlgos::bingham2Tensor( ds );
            }
            break;
        }
        case FNALGO_TEST:
        {
            Dataset* ds = VPtr<Dataset>::asPtr( model()->data( index, Qt::EditRole ) );
            if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SCALAR )
            {
                //DWIAlgos::testAlgo( ds, m_datasetList );
            }
            break;
        }
    }
    for ( int i = 0; i < l.size(); ++i )
    {
        model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ) );
    }
}
