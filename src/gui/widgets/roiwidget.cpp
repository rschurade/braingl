/*
 * roiwidget.cpp
 *
 * Created on: Jan 30, 2013
 * @author Ralph Schurade
 */
#include "roiwidget.h"

#include "../views/roitreeview.h"

#include "../../data/models.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"


#include <QDebug>
#include <QtGui>

ROIWidget::ROIWidget( QWidget* parent ) :
    QFrame( parent ),
    m_count( 0 )
{
    setMaximumHeight( 300 );

    m_treeView = new ROITreeView( this );
    m_treeView->setModel( Models::r() );
    m_treeView->hideColumn( 1 );
    connect( m_treeView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeClicked() ) );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
//    m_newTopButton = new QPushButton( this );
//    m_newTopButton->setText( QString( "new top" ) );
//    m_newChildButton = new QPushButton( this );
//    m_newChildButton->setText( QString( "new child" ) );
    m_deleteButton = new QPushButton( this );
    m_deleteButton->setText( QString( "delete" ) );

    //buttonLayout->addWidget( m_newTopButton );
    //buttonLayout->addWidget( m_newChildButton );
    buttonLayout->addWidget( m_deleteButton );

//    connect( m_upButton, SIGNAL( clicked() ), this, SLOT( moveItemUp() ) );
//    connect( m_downButton, SIGNAL( clicked() ), this, SLOT( moveItemDown() ) );
    connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteItem() ) );

    buttonLayout->setContentsMargins( 0, 0, 0, 0 );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget( m_treeView );
    vLayout->addLayout( buttonLayout );
    vLayout->setContentsMargins( 1, 1, 1, 1 );

    vLayout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( vLayout );
    setFrameStyle( QFrame::StyledPanel | QFrame::Plain );


//    m_newTopButton->setEnabled( false );
//    m_newChildButton->setEnabled( false );
    m_deleteButton->setEnabled( false );

    setStyleSheet( "QPushButton { font:  bold 12px; max-height: 14px; margin-top: -1px } " );

    connect( m_treeView, SIGNAL(itemSelectionChanged( const QItemSelection ) ), this, SLOT( itemSelectionChanged( const QItemSelection ) ) );
}

ROIWidget::~ROIWidget()
{
}

void ROIWidget::addBox()
{
    m_treeView->addBox();
}

void ROIWidget::addROIArea( ROIArea* roi )
{
    m_treeView->addROIArea( roi );
}


void ROIWidget::treeClicked()
{
    setFocus();
}

QItemSelectionModel* ROIWidget::selectionModel()
{
    return m_treeView->selectionModel();
}

void ROIWidget::itemSelectionChanged( const QItemSelection &selected )
{
    m_deleteButton->setEnabled( false );

    if ( selected.indexes().size() > 0 )
    {
        m_deleteButton->setEnabled( true );
    }
}

void ROIWidget::deleteItem()
{
    QModelIndex id = m_treeView->selectionModel()->selectedIndexes().first();
    QModelIndex index = Models::r()->index( id.row(), (int)Fn::Property::D_POINTER, Models::r()->parent( id ) );
    ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( index, Qt::DisplayRole ) );
    delete roi;
    m_treeView->model()->removeRows( id.row(), 0, id.parent() );
}

void ROIWidget::keyPressEvent( QKeyEvent* event )
{
    if ( m_treeView->selectionModel()->selectedIndexes().empty() )
    {
        return;
    }
    QModelIndex id = m_treeView->selectionModel()->selectedIndexes().first();

    QModelIndex index = Models::r()->index( id.row(), (int)Fn::Property::D_POINTER, Models::r()->parent( id ) );

    ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( index, Qt::DisplayRole ) );
    int type = roi->properties()->get( Fn::Property::D_SHAPE ).toInt();
    if ( type < 10 )
    {
        float add = 1.0f;

        if ( event->modifiers() & Qt::ShiftModifier )
        {
            add = 5.0f;
        }

        float rx = roi->properties()->get( Fn::Property::D_X ).toFloat();
        float ry = roi->properties()->get( Fn::Property::D_Y ).toFloat();
        float rz = roi->properties()->get( Fn::Property::D_Z ).toFloat();
        float rdx = roi->properties()->get( Fn::Property::D_DX ).toFloat();
        float rdy = roi->properties()->get( Fn::Property::D_DY ).toFloat();
        float rdz = roi->properties()->get( Fn::Property::D_DZ ).toFloat();
        float dim = 250;

        switch( event->key() )
        {
            case Qt::Key_Left:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdx = qMax( 0.5f, qMin( rdx - add, dim ) );
                }
                else
                {
                    rx = qMax( 0.0f, qMin( rx - add, dim ) );
                }
                break;
            }
            case Qt::Key_Right:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdx = qMax( 0.5f, qMin( rdx + add, dim ) );
                }
                else
                {
                    rx = qMax( 0.0f, qMin( rx + add, dim ) );
                }
                break;
            }
            case Qt::Key_Up:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdy = qMax( 0.5f, qMin( rdy + add, dim ) );
                }
                else
                {
                    ry = qMax( 0.0f, qMin( ry + add, dim ) );
                }
                break;
            }
            case Qt::Key_Down:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdy = qMax( 0.5f, qMin( rdy - add, dim ) );
                }
                else
                {
                    ry = qMax( 0.0f, qMin( ry - add, dim ) );
                }
                break;
            }
            case Qt::Key_PageUp:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdz = qMax( 0.5f, qMin( rdz + add, dim ) );
                }
                else
                {
                    rz = qMax( 0.0f, qMin( rz + add, dim ) );
                }
                break;
            }
            case Qt::Key_PageDown:
            {
                if ( event->modifiers() & Qt::ControlModifier )
                {
                    rdz = qMax( 0.5f, qMin( rdz - add, dim ) );
                }
                else
                {
                    rz = qMax( 0.0f, qMin( rz - add, dim ) );
                }
                break;
            }
        }
        roi->properties()->set( Fn::Property::D_X, rx );
        roi->properties()->set( Fn::Property::D_Y, ry );
        roi->properties()->set( Fn::Property::D_Z, rz );
        roi->properties()->set( Fn::Property::D_DX, rdx );
        roi->properties()->set( Fn::Property::D_DY, rdy );
        roi->properties()->set( Fn::Property::D_DZ, rdz );
    }
    Models::r()->setData( Models::r()->index( id.row(), (int)Fn::Property::D_UPDATED, Models::r()->parent( id ) ), true );
}
