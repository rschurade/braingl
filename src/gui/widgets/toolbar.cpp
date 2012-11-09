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
    m_scalarAction1 = new QAction( QIcon( ":/icons/tmp1.png" ), tr( "Scalar 1" ), this );
    m_scalarAction1->setStatusTip( tr( "Scalar Action 1" ) );
    connect( m_scalarAction1, SIGNAL( triggered() ), this, SLOT( slotScalar1() ) );

    m_vectorAction1 = new QAction( QIcon( ":/icons/tmp3.png" ), tr( "Vector 1" ), this );
    m_vectorAction1->setStatusTip( tr( "Vector Action 1" ) );
    connect( m_vectorAction1, SIGNAL( triggered() ), this, SLOT( slotVector1() ) );

    m_multiAction1 = new QAction( QIcon( ":/icons/qball4.png" ), tr( "QBall 4th order" ), this );
    m_multiAction1->setStatusTip( tr( "QBall 4th order" ) );
    connect( m_multiAction1, SIGNAL( triggered() ), this, SLOT( slotMulti1() ) );

    m_multiAction1_2 = new QAction( QIcon( ":/icons/qball6.png" ), tr( "QBall 6th order" ), this );
    m_multiAction1_2->setStatusTip( tr( "QBall 6th order" ) );
    connect( m_multiAction1_2, SIGNAL( triggered() ), this, SLOT( slotMulti1_2() ) );

    m_multiAction1_3 = new QAction( QIcon( ":/icons/qball8.png" ), tr( "QBall 8th order" ), this );
    m_multiAction1_3->setStatusTip( tr( "QBall 8th order" ) );
    connect( m_multiAction1_3, SIGNAL( triggered() ), this, SLOT( slotMulti1_3() ) );



    m_multiAction2 = new QAction( QIcon( ":/icons/fittensor.png" ), tr( "Tensor fit" ), this );
    m_multiAction2->setStatusTip( tr( "tensor fit" ) );
    connect( m_multiAction2, SIGNAL( triggered() ), this, SLOT( slotMulti2() ) );

    m_multiAction3 = new QAction( QIcon( ":/icons/calcfa.png" ), tr( "Calc FA" ), this );
    m_multiAction3->setStatusTip( tr( "calc FA" ) );
    connect( m_multiAction3, SIGNAL( triggered() ), this, SLOT( slotMulti3() ) );

    m_multiAction4 = new QAction( QIcon( ":/icons/eigenvector.png" ), tr( "Calc Eigen Vector" ), this );
    m_multiAction4->setStatusTip( tr( "calc Eigen Vector" ) );
    connect( m_multiAction4, SIGNAL( triggered() ), this, SLOT( slotMulti4() ) );

    m_meshAction1 = new QAction( QIcon( ":/icons/tmpm.png" ), tr( "Mesh 1" ), this );
    m_meshAction1->setStatusTip( tr( "Mesh Action 1" ) );
    connect( m_meshAction1, SIGNAL( triggered() ), this, SLOT( slotMesh1() ) );

    m_fiberAction1 = new QAction( QIcon( ":/icons/tmpf.png" ), tr( "Fiber 1" ), this );
    m_fiberAction1->setStatusTip( tr( "Fiber Action 1" ) );
    connect( m_fiberAction1, SIGNAL( triggered() ), this, SLOT( slotFiber1() ) );
}

void ToolBar::slotScalar1()
{
    qDebug() << "scalar button pressed";
}

void ToolBar::slotVector1()
{
    qDebug() << "vector button pressed";
}

void ToolBar::slotMulti1()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL4 );
}

void ToolBar::slotMulti1_2()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL6 );
}

void ToolBar::slotMulti1_3()
{
    m_toolBarView->activateAlgo( FNALGO_QBALL8 );
}


void ToolBar::slotMulti2()
{
    m_toolBarView->activateAlgo( FNALGO_TENSORFIT );
}

void ToolBar::slotMulti3()
{
    m_toolBarView->activateAlgo( FNALGO_FA );
}

void ToolBar::slotMulti4()
{
    m_toolBarView->activateAlgo( FNALGO_EV );
}



void ToolBar::slotMesh1()
{
    qDebug() << "mesh button pressed";
}

void ToolBar::slotFiber1()
{
    qDebug() << "fiber button pressed";
}

void ToolBar::slotSelectionChanged( int type )
{
    this->clear();
    switch ( type )
    {
        case FNDT_NIFTI_SCALAR:
        {
            this->addAction( m_scalarAction1 );
            break;
        }
        case FNDT_NIFTI_VECTOR:
        {
            this->addAction( m_vectorAction1 );
            break;
        }
        case FNDT_NIFTI_DWI:
        {
            this->addAction( m_multiAction1 );
            this->addAction( m_multiAction1_2 );
            this->addAction( m_multiAction1_3 );
            this->addAction( m_multiAction2 );
            this->addAction( m_multiAction3 );
            this->addAction( m_multiAction4 );
            break;
        }
        default:
        {
            break;
        }
    }
}
