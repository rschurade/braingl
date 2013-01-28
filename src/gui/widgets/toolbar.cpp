/*
 * toolbar.cpp
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */
#include <QtCore/QVariant>

#include "../../data/vptr.h"
#include "../../data/dwialgos.h"
#include "../../data/scalaralgos.h"
#include "../../data/fiberalgos.h"
#include "../../data/datasets/dataset.h"

#include "../views/toolbarview.h"
#include "../widgets/tensortrackwidget.h"

#include "toolbar.h"

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
    m_isosurfaceAct = new FNAction( QIcon( ":/icons/tmp1.png" ), tr( "Scalar 1" ), this, FNALGO_ISOSURFACE );
    m_isosurfaceAct->setStatusTip( tr( "Scalar Action 1" ) );
    connect( m_isosurfaceAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_testAct = new FNAction( QIcon( ":/icons/tmp3.png" ), tr( "Test" ), this, FNALGO_TEST );
    m_testAct->setStatusTip( tr( "Test Action 1" ) );
    connect( m_testAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_qball4Act = new FNAction( QIcon( ":/icons/qball4.png" ), tr( "QBall 4th order" ), this, FNALGO_QBALL4 );
    m_qball4Act->setStatusTip( tr( "QBall 4th order" ) );
    connect( m_qball4Act, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_qball6Act = new FNAction( QIcon( ":/icons/qball6.png" ), tr( "QBall 6th order" ), this, FNALGO_QBALL6 );
    m_qball6Act->setStatusTip( tr( "QBall 6th order" ) );
    connect( m_qball6Act, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_qball8Act = new FNAction( QIcon( ":/icons/qball8.png" ), tr( "QBall 8th order" ), this, FNALGO_QBALL8 );
    m_qball8Act->setStatusTip( tr( "QBall 8th order" ) );
    connect( m_qball8Act, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_tensorAct = new FNAction( QIcon( ":/icons/fittensor.png" ), tr( "Tensor fit" ), this, FNALGO_TENSORFIT );
    m_tensorAct->setStatusTip( tr( "tensor fit" ) );
    connect( m_tensorAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_faAct = new FNAction( QIcon( ":/icons/calcfa.png" ), tr( "Calc FA" ), this, FNALGO_FA );
    m_faAct->setStatusTip( tr( "calc FA" ) );
    connect( m_faAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_evAct = new FNAction( QIcon( ":/icons/eigenvector.png" ), tr( "Calc Eigen Vector" ), this, FNALGO_EV );
    m_evAct->setStatusTip( tr( "calc Eigen Vector" ) );
    connect( m_evAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_binghamAction = new FNAction( QIcon( ":/icons/tmpx.png" ), tr( "Fit Bingham" ), this, FNALGO_BINGHAM );
    m_binghamAction->setStatusTip( tr( "fit bingham distribution" ) );
    connect( m_binghamAction, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_fiberTrackingAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "tensor tracking" ), this, FNALGO_TENSOR_TRACK );
    m_fiberTrackingAct->setStatusTip( tr( "tensor tracking" ) );
    connect( m_fiberTrackingAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_fiberThinningAct = new FNAction( QIcon( ":/icons/tmpf.png" ), tr( "fiber thinning" ), this, FNALGO_FIBER_THINNING );
    m_fiberThinningAct->setStatusTip( tr( "fiber thinning" ) );
    connect( m_fiberThinningAct, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );

    m_bingham2tensorAction = new FNAction( QIcon( ":/icons/tmpx.png" ), tr( "Bingham 2 Tensor" ), this, FNALGO_BINGHAM_2_TENSOR );
    m_bingham2tensorAction->setStatusTip( tr( "create tensors from bingham fit" ) );
    connect( m_bingham2tensorAction, SIGNAL( sigTriggered( FN_ALGO ) ), this, SLOT( slot( FN_ALGO ) ) );
}

void ToolBar::slot( FN_ALGO algo )
{
    //m_toolBarView->activateAlgo( algo );

    QModelIndex index = m_toolBarView->model()->index( m_toolBarView->getSelected(), FNPROP_DATASET_POINTER );
    QList<Dataset*>l;
    Dataset* ds = VPtr<Dataset>::asPtr( m_toolBarView->model()->data( index, Qt::EditRole ) );
    switch ( algo )
    {
        case FNALGO_QBALL4:
            l = DWIAlgos::qBallSharp( ds, 4 );
            break;
        case FNALGO_QBALL6:
            l = DWIAlgos::qBallSharp( ds, 6 );
            break;
        case FNALGO_QBALL8:
            l = DWIAlgos::qBallSharp( ds, 8 );
            break;
        case FNALGO_TENSORFIT:
            l = DWIAlgos::tensorFit( ds );
            break;
        case FNALGO_FA:
            l = DWIAlgos::calcFAFromDWI( ds );
            break;
        case FNALGO_EV:
            l = DWIAlgos::calcEVFromDWI( ds );
            break;
        case FNALGO_BINGHAM:
            l = DWIAlgos::fitBingham( ds );
            break;
        case FNALGO_TENSOR_TRACK:
        {
            QList<QVariant>dsList =  m_toolBarView->model()->data( m_toolBarView->model()->index( 0, FNPROP_DATASET_LIST ), Qt::EditRole ).toList();

            m_ttw = new TensorTrackWidget( ds, dsList, this->parentWidget() );
            connect( m_ttw, SIGNAL( finished() ), this, SLOT( tensorTrackFinished() ) );
            m_ttw->show();
            break;

        }
        case FNALGO_ISOSURFACE:
            l = ScalarAlgos::isoSurface( ds );
            break;
        case FNALGO_FIBER_THINNING:
            l = FiberAlgos::thinOut( ds );
            break;
        case FNALGO_BINGHAM_2_TENSOR:
            l = DWIAlgos::bingham2DWI( ds );
            break;
        case FNALGO_TEST:
            break;
        case FNALGO_NONE:
            break;
        case FNALGO_QBALL:
            break;
    }
    for ( int i = 0; i < l.size(); ++i )
    {
        m_toolBarView->model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ) );
    }

}

void ToolBar::slotSelectionChanged( int type )
{
    this->clear();
    switch ( type )
    {
        case FNDT_NIFTI_SCALAR:
        {
            this->addAction( m_isosurfaceAct );
            break;
        }
        case FNDT_NIFTI_VECTOR:
        {
            //this->addAction( m_vectorAction1 );
            break;
        }
        case FNDT_NIFTI_TENSOR:
        {
            this->addAction( m_faAct );
            this->addAction( m_evAct );
            this->addAction( m_fiberTrackingAct );
            break;
        }
        case FNDT_NIFTI_SH:
        {
            this->addAction( m_binghamAction );
            break;
        }
        case FNDT_NIFTI_DWI:
        {
            this->addAction( m_qball4Act );
            this->addAction( m_qball6Act );
            this->addAction( m_qball8Act );
            this->addAction( m_tensorAct );
            this->addAction( m_faAct );
            this->addAction( m_evAct );
            break;
        }
        case FNDT_NIFTI_BINGHAM:
        {
            this->addAction( m_bingham2tensorAction );
            break;
        }
        case FNDT_FIBERS:
        {
            this->addAction( m_fiberThinningAct );
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
    QModelIndex index = m_toolBarView->model()->index( m_toolBarView->getSelected(), FNPROP_DATASET_POINTER );
    QList<Dataset*>l = m_ttw->getFibs();
    for ( int i = 0; i < l.size(); ++i )
   {
       m_toolBarView->model()->setData( index, VPtr<Dataset>::asQVariant( l[i] ) );
   }
    m_ttw->hide();
    destroy( m_ttw );
}

