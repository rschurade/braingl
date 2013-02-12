/*
 * toolbar.cpp
 *
 *  Created on: 14.06.2012
 *      Author: Ralph Schurade
 */
#include "toolbar.h"

#include "../views/toolbarview.h"
#include "../widgets/algoStarterWidgets/tensortrackwidget.h"
#include "../widgets/algoStarterWidgets/crossingtrackwidget.h"

#include "../../algos/dwialgos.h"
#include "../../algos/scalaralgos.h"
#include "../../algos/fiberalgos.h"
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"

#include <QtCore/QVariant>

ToolBar::ToolBar( const QString &title, QWidget* parent ) :
    QToolBar( title, parent )
{
    setObjectName( title );
    m_toolBarView = new ToolBarView( this );

    connect( m_toolBarView, SIGNAL( sigSelectionChanged( int ) ), this, SLOT( slotSelectionChanged( int ) ) );

    createActions();
}

ToolBar::~ToolBar()
{
}

void ToolBar::setModel( QAbstractItemModel* model )
{
    m_toolBarView->setModel( model );

}

void ToolBar::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_toolBarView->setSelectionModel( selectionModel );
}

void ToolBar::createActions()
{
    m_isosurfaceAct = new FNAction( QIcon( ":/icons/iso_surface.png" ), tr( "iso surface" ), this, Fn::Algo::ISOSURFACE );
    m_isosurfaceAct->setStatusTip( tr( "iso surface" ) );
    connect( m_isosurfaceAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_testAct = new FNAction( QIcon( ":/icons/tmp3.png" ), tr( "Test" ), this, Fn::Algo::TEST );
    m_testAct->setStatusTip( tr( "Test Action 1" ) );
    connect( m_testAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_qball4Act = new FNAction( QIcon( ":/icons/qball4.png" ), tr( "QBall 4th order" ), this, Fn::Algo::QBALL4 );
    m_qball4Act->setStatusTip( tr( "QBall 4th order" ) );
    connect( m_qball4Act, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_qball6Act = new FNAction( QIcon( ":/icons/qball6.png" ), tr( "QBall 6th order" ), this, Fn::Algo::QBALL6 );
    m_qball6Act->setStatusTip( tr( "QBall 6th order" ) );
    connect( m_qball6Act, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_qball8Act = new FNAction( QIcon( ":/icons/qball8.png" ), tr( "QBall 8th order" ), this, Fn::Algo::QBALL8 );
    m_qball8Act->setStatusTip( tr( "QBall 8th order" ) );
    connect( m_qball8Act, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_tensorAct = new FNAction( QIcon( ":/icons/fittensor.png" ), tr( "Tensor fit" ), this, Fn::Algo::TENSORFIT );
    m_tensorAct->setStatusTip( tr( "tensor fit" ) );
    connect( m_tensorAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_faAct = new FNAction( QIcon( ":/icons/calcfa.png" ), tr( "Calc FA" ), this, Fn::Algo::FA );
    m_faAct->setStatusTip( tr( "calc FA" ) );
    connect( m_faAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_evAct = new FNAction( QIcon( ":/icons/eigenvector.png" ), tr( "Calc Eigen Vector" ), this, Fn::Algo::EV );
    m_evAct->setStatusTip( tr( "calc Eigen Vector" ) );
    connect( m_evAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_binghamAction = new FNAction( QIcon( ":/icons/tmpx.png" ), tr( "Fit Bingham" ), this, Fn::Algo::BINGHAM );
    m_binghamAction->setStatusTip( tr( "fit bingham distribution" ) );
    connect( m_binghamAction, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_fiberTrackingAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "tensor tracking" ), this, Fn::Algo::TENSOR_TRACK );
    m_fiberTrackingAct->setStatusTip( tr( "tensor tracking" ) );
    connect( m_fiberTrackingAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_crossingTrackingAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "tensor tracking with fiber crossings" ), this, Fn::Algo::CROSSING_TRACK );
    m_crossingTrackingAct->setStatusTip( tr( "tensor tracking with crossings" ) );
    connect( m_crossingTrackingAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_fiberThinningAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "fiber thinning" ), this, Fn::Algo::FIBER_THINNING );
    m_fiberThinningAct->setStatusTip( tr( "fiber thinning" ) );
    connect( m_fiberThinningAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_fiberTractDensityAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "tract density" ), this, Fn::Algo::TRACT_DENSITY );
    m_fiberTractDensityAct->setStatusTip( tr( "tract density" ) );
    connect( m_fiberTractDensityAct, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_bingham2tensorAction = new FNAction( QIcon( ":/icons/tmpx.png" ), tr( "Bingham 2 Tensor" ), this, Fn::Algo::BINGHAM_2_TENSOR );
    m_bingham2tensorAction->setStatusTip( tr( "create tensors from bingham fit" ) );
    connect( m_bingham2tensorAction, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );

    m_cutSelectedFibersAction = new FNAction( QIcon( ":/icons/cut.png" ), tr( "Cut selected fibers" ), this, Fn::Algo::CUT_SELECTED_FIBERS );
    m_cutSelectedFibersAction->setStatusTip( tr( "create new fiber dataset from selected fibers" ) );
    connect( m_cutSelectedFibersAction, SIGNAL( sigTriggered( Fn::Algo ) ), this, SLOT( slot( Fn::Algo ) ) );
}

void ToolBar::slot( Fn::Algo algo )
{
    //m_toolBarView->activateAlgo( algo );

    QModelIndex index = m_toolBarView->model()->index( m_toolBarView->getSelected(), (int)Fn::Property::DATASET_POINTER );
    QList<Dataset*>l;
    Dataset* ds = VPtr<Dataset>::asPtr( m_toolBarView->model()->data( index, Qt::DisplayRole ) );
    switch ( algo )
    {
        case Fn::Algo::QBALL4:
            l = DWIAlgos::qBallSharp( ds, 4 );
            break;
        case Fn::Algo::QBALL6:
            l = DWIAlgos::qBallSharp( ds, 6 );
            break;
        case Fn::Algo::QBALL8:
            l = DWIAlgos::qBallSharp( ds, 8 );
            break;
        case Fn::Algo::TENSORFIT:
            l = DWIAlgos::tensorFit( ds );
            break;
        case Fn::Algo::FA:
            if ( ds->properties()->get( Fn::Property::TYPE ) == (int)Fn::DatasetType::NIFTI_DWI )
            {
                l = DWIAlgos::calcFAFromDWI( ds );
            }
            else if ( ds->properties()->get( Fn::Property::TYPE ) == (int)Fn::DatasetType::NIFTI_TENSOR )
            {
                l = DWIAlgos::calcFAFromTensor( ds );
            }
            break;
        case Fn::Algo::EV:
            if ( ds->properties()->get( Fn::Property::TYPE ) == (int)Fn::DatasetType::NIFTI_DWI )
            {
                l = DWIAlgos::calcEVFromDWI( ds );
            }
            else if ( ds->properties()->get( Fn::Property::TYPE ) == (int)Fn::DatasetType::NIFTI_TENSOR )
            {
                l = DWIAlgos::calcEVFromTensor( ds );
            }
            break;
        case Fn::Algo::BINGHAM:
            l = DWIAlgos::fitBingham( ds );
            break;
        case Fn::Algo::TENSOR_TRACK:
        {
            QList<QVariant>dsList =  m_toolBarView->model()->data( m_toolBarView->model()->index( 0, (int)Fn::Property::DATASET_LIST ), Qt::DisplayRole ).toList();

            m_ttw = new TensorTrackWidget( ds, dsList, this->parentWidget() );
            connect( m_ttw, SIGNAL( finished() ), this, SLOT( tensorTrackFinished() ) );
            m_ttw->show();
            break;

        }
        case Fn::Algo::CROSSING_TRACK:
        {
            QList<QVariant>dsList =  m_toolBarView->model()->data( m_toolBarView->model()->index( 0, (int)Fn::Property::DATASET_LIST ), Qt::DisplayRole ).toList();

            m_ctw = new CrossingTrackWidget( ds, dsList, this->parentWidget() );
            connect( m_ctw, SIGNAL( finished() ), this, SLOT( crossingTrackFinished() ) );
            m_ctw->show();
            break;

        }

        case Fn::Algo::ISOSURFACE:
            l = ScalarAlgos::isoSurface( ds );
            break;
        case Fn::Algo::FIBER_THINNING:
            l = FiberAlgos::thinOut( ds );
            break;
        case Fn::Algo::TRACT_DENSITY:
            l = FiberAlgos::tractDensity( ds );
            break;
        case Fn::Algo::BINGHAM_2_TENSOR:
            l = DWIAlgos::bingham2DWI( ds );
            break;
        case Fn::Algo::TEST:
            break;
        case Fn::Algo::NONE:
            break;
        case Fn::Algo::QBALL:
            break;
        case Fn::Algo::CUT_SELECTED_FIBERS:
            if ( ds->properties()->get( Fn::Property::TYPE ) == (int)Fn::DatasetType::FIBERS )
            {
                l = FiberAlgos::cutSelecteded( ds );
            }
            break;
    }
    for ( int i = 0; i < l.size(); ++i )
    {
        index = m_toolBarView->model()->index( m_toolBarView->model()->rowCount(), (int)Fn::Property::NEW_DATASET );
        m_toolBarView->model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ), Qt::DisplayRole );
    }

}

void ToolBar::slotSelectionChanged( int type )
{
    this->clear();
    switch ( (Fn::DatasetType)type )
    {
        case Fn::DatasetType::NIFTI_SCALAR:
        {
            this->addAction( m_isosurfaceAct );
            break;
        }
        case Fn::DatasetType::NIFTI_VECTOR:
        {
            //this->addAction( m_vectorAction1 );
            break;
        }
        case Fn::DatasetType::NIFTI_TENSOR:
        {
            this->addAction( m_faAct );
            this->addAction( m_evAct );
            this->addAction( m_fiberTrackingAct );
            this->addAction( m_crossingTrackingAct );
            break;
        }
        case Fn::DatasetType::NIFTI_SH:
        {
            this->addAction( m_binghamAction );
            break;
        }
        case Fn::DatasetType::NIFTI_DWI:
        {
            this->addAction( m_qball4Act );
            this->addAction( m_qball6Act );
            this->addAction( m_qball8Act );
            this->addAction( m_tensorAct );
            this->addAction( m_faAct );
            this->addAction( m_evAct );
            break;
        }
        case Fn::DatasetType::NIFTI_BINGHAM:
        {
            this->addAction( m_bingham2tensorAction );
            break;
        }
        case Fn::DatasetType::FIBERS:
        {
            this->addAction( m_cutSelectedFibersAction );
            this->addAction( m_fiberThinningAct );
            this->addAction( m_fiberTractDensityAct );
            break;
        }
        default:
        {
            break;
        }
    }
}

void ToolBar::tensorTrackFinished()
{
    qDebug() << "toolbar track finished";
    QList<Dataset*>l = m_ttw->getFibs();
    for ( int i = 0; i < l.size(); ++i )
    {
        QModelIndex index = m_toolBarView->model()->index( m_toolBarView->model()->rowCount(), (int)Fn::Property::NEW_DATASET );
        m_toolBarView->model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ), Qt::DisplayRole );
    }
    m_ttw->hide();
    destroy( m_ttw );
}

void ToolBar::crossingTrackFinished()
{
    qDebug() << "toolbar crossing track finished";
    QList<Dataset*>l = m_ctw->getFibs();
    for ( int i = 0; i < l.size(); ++i )
    {
        QModelIndex index = m_toolBarView->model()->index( m_toolBarView->model()->rowCount(), (int)Fn::Property::NEW_DATASET );
        m_toolBarView->model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ), Qt::DisplayRole );
    }
    m_ctw->hide();
    destroy( m_ctw );
}
