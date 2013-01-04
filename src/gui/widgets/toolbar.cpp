/*
 * toolbar.cpp
 *
 *  Created on: 14.06.2012
 *      Author: Ralph
 */
#include "../views/toolbarview.h"
#include "../../data/datasets/dataset.h"

#include "../../data/enums.h"

#include "toolbar.h"

ToolBar::ToolBar( const QString &title, QWidget* parent ) :
    QToolBar( title, parent )
{
    setObjectName( title );
    m_toolBarView = new ToolBarView();

    connect( m_toolBarView, SIGNAL( sigSelectionChanged( int ) ), this, SLOT( slotSelectionChanged( int ) ) );

    createActions();

//    this->addAction( m_scalarAction1 );
//    this->addAction( m_vectorAction1 );
//    this->addAction( m_multiAction1 );
//    this->addAction( m_meshAction1 );
//    this->addAction( m_fiberAction1 );
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
    m_isosurfaceAct = new QAction( QIcon( ":/icons/tmp1.png" ), tr( "Scalar 1" ), this );
    m_isosurfaceAct->setStatusTip( tr( "Scalar Action 1" ) );
    connect( m_isosurfaceAct, SIGNAL( triggered() ), this, SLOT( slotIsosurface() ) );

    m_vectorAction1 = new QAction( QIcon( ":/icons/tmp3.png" ), tr( "Vector 1" ), this );
    m_vectorAction1->setStatusTip( tr( "Vector Action 1" ) );
    connect( m_vectorAction1, SIGNAL( triggered() ), this, SLOT( slotVector1() ) );

    m_qball4Act = new QAction( QIcon( ":/icons/qball4.png" ), tr( "QBall 4th order" ), this );
    m_qball4Act->setStatusTip( tr( "QBall 4th order" ) );
    connect( m_qball4Act, SIGNAL( triggered() ), this, SLOT( slotQBall4() ) );

    m_qball6Act = new QAction( QIcon( ":/icons/qball6.png" ), tr( "QBall 6th order" ), this );
    m_qball6Act->setStatusTip( tr( "QBall 6th order" ) );
    connect( m_qball6Act, SIGNAL( triggered() ), this, SLOT( slotQBall6() ) );

    m_qball8Act = new QAction( QIcon( ":/icons/qball8.png" ), tr( "QBall 8th order" ), this );
    m_qball8Act->setStatusTip( tr( "QBall 8th order" ) );
    connect( m_qball8Act, SIGNAL( triggered() ), this, SLOT( slotQBall8() ) );



    m_tensorAct = new QAction( QIcon( ":/icons/fittensor.png" ), tr( "Tensor fit" ), this );
    m_tensorAct->setStatusTip( tr( "tensor fit" ) );
    connect( m_tensorAct, SIGNAL( triggered() ), this, SLOT( slotTensor() ) );

    m_faAct = new QAction( QIcon( ":/icons/calcfa.png" ), tr( "Calc FA" ), this );
    m_faAct->setStatusTip( tr( "calc FA" ) );
    connect( m_faAct, SIGNAL( triggered() ), this, SLOT( slotFA() ) );

    m_evAct = new QAction( QIcon( ":/icons/eigenvector.png" ), tr( "Calc Eigen Vector" ), this );
    m_evAct->setStatusTip( tr( "calc Eigen Vector" ) );
    connect( m_evAct, SIGNAL( triggered() ), this, SLOT( slotEV() ) );

    m_binghamAction = new QAction( QIcon( ":/icons/tmpx.png" ), tr( "Fit Bingham" ), this );
    m_binghamAction->setStatusTip( tr( "fit bingham distribution" ) );
    connect( m_binghamAction, SIGNAL( triggered() ), this, SLOT( slotBingham() ) );

    m_meshAction1 = new QAction( QIcon( ":/icons/tmpm.png" ), tr( "Mesh 1" ), this );
    m_meshAction1->setStatusTip( tr( "Mesh Action 1" ) );
    connect( m_meshAction1, SIGNAL( triggered() ), this, SLOT( slotMesh1() ) );

    m_fiberAction1 = new QAction( QIcon( ":/icons/tmpf.png" ), tr( "Fiber 1" ), this );
    m_fiberAction1->setStatusTip( tr( "Fiber Action 1" ) );
    connect( m_fiberAction1, SIGNAL( triggered() ), this, SLOT( slotFiber1() ) );
}

void ToolBar::slotIsosurface()
{
    m_toolBarView->activateAlgo( FNALGO_ISOSURFACE );
}

void ToolBar::slotVector1()
{
    qDebug() << "vector button pressed";
}

void ToolBar::slotQBall4()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL4 );
}

void ToolBar::slotQBall6()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL6 );
}

void ToolBar::slotQBall8()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL8 );
}


void ToolBar::slotTensor()
{
    m_toolBarView->activateAlgo( FNALGO_TENSORFIT );
}

void ToolBar::slotFA()
{
    m_toolBarView->activateAlgo( FNALGO_FA );
}

void ToolBar::slotEV()
{
    m_toolBarView->activateAlgo( FNALGO_EV );
}

void ToolBar::slotBingham()
{
    m_toolBarView->activateAlgo( FNALGO_BINGHAM );
}


void ToolBar::slotMesh1()
{
    qDebug() << "mesh button pressed";
}

void ToolBar::slotFiber1()
{
    m_toolBarView->activateAlgo( FNALGO_TENSOR_TRACK );
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
            this->addAction( m_fiberAction1 );
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
        default:
        {
            break;
        }
    }
}
