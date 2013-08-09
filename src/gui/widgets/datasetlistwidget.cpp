/*
 * datasetviewwidget.cpp
 *
 * Created on: 08.05.2012
 * @author Ralph Schurade
 */
#include "datasetlistwidget.h"

#include "../views/datasetlistview.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"

#include <QtGui>

DatasetListWidget::DatasetListWidget( QWidget* parent ) :
	QWidget( parent ),
	m_selected( -1 )
{
    setMaximumHeight( 200 );

	QFrame* frame = new QFrame( this );

	QVBoxLayout* layout = new QVBoxLayout();

	m_listView = new DatasetListView( frame );
	m_listView->setModel( Models::d() );
	m_listView->setToolTip( QString( "datasets" ) );
	m_listView->setStyleSheet( "border: none" );
    layout->addWidget( m_listView );

    layout->setContentsMargins( 0, 0, 0, 0 );
    frame->setLayout( layout );
    frame->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

	m_layout = new QVBoxLayout();

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	m_downButton = new QPushButton( this );
	m_downButton->setText( QString( "down" ) );
	m_upButton = new QPushButton( this );
	m_upButton->setText( QString( "up" ) );
	m_deleteButton = new QPushButton( this );
	m_deleteButton->setText( QString( "delete" ) );

	buttonLayout->addWidget( m_downButton );
	buttonLayout->addWidget( m_upButton );
	buttonLayout->addWidget( m_deleteButton );

	connect( m_upButton, SIGNAL( clicked() ), this, SLOT( moveItemUp() ) );
	connect( m_downButton, SIGNAL( clicked() ), this, SLOT( moveItemDown() ) );
	connect( m_deleteButton, SIGNAL( clicked() ), this, SLOT( deleteItem() ) );

	connect( m_listView, SIGNAL(itemSelectionChanged( const QItemSelection ) ), this, SLOT( itemSelectionChanged( const QItemSelection ) ) );

	buttonLayout->setContentsMargins( 0, 0, 0, 0 );

	m_layout->addWidget( frame );
	m_layout->addLayout( buttonLayout );
	m_layout->setContentsMargins( 1, 1, 1, 1 );

	setLayout( m_layout );

	m_upButton->setEnabled( false );
	m_downButton->setEnabled( false );
	m_deleteButton->setEnabled( false );

	setStyleSheet( "QPushButton { font:  bold 12px; max-height: 14px; margin-top: -1px } " );
}

DatasetListWidget::~DatasetListWidget()
{
}

QItemSelectionModel* DatasetListWidget::selectionModel()
{
    return m_listView->selectionModel();
}

void DatasetListWidget::moveItemDown()
{
	emit moveSelectedItemDown( m_selected );
	QItemSelectionModel* selector = m_listView->selectionModel();
	QAbstractItemModel* model = m_listView->model();

	int toSelect = m_selected + 1;
	if ( m_selected > -1 && m_selected < m_listView->model()->rowCount() )
	{
	    selector->select( model->index( m_selected, 0 ), QItemSelectionModel::Deselect );
	    selector->select( model->index( toSelect, 0 ), QItemSelectionModel::Select );
    }
}

void DatasetListWidget::moveItemUp()
{
	emit moveSelectedItemUp( m_selected );
    QItemSelectionModel* selector = m_listView->selectionModel();
    QAbstractItemModel* model = m_listView->model();

    int toSelect = m_selected - 1;
    if ( m_selected > 0 )
    {
        selector->select( model->index( m_selected, 0 ), QItemSelectionModel::Deselect );
        selector->select( model->index( toSelect, 0 ), QItemSelectionModel::Select );
    }
}


void DatasetListWidget::itemSelectionChanged( const QItemSelection &selected )
{
	m_upButton->setEnabled( false );
	m_downButton->setEnabled( false );
	m_deleteButton->setEnabled( false );

	if ( selected.indexes().size() > 0 )
	{
	    m_selected = selected.indexes().first().row();
	    m_deleteButton->setEnabled( true );
	}
	else
	{
	    m_selected = -1;
	}

	if ( m_selected > 0 )
	{
		m_upButton->setEnabled( true );
	}
	if ( m_selected < m_listView->model()->rowCount() - 1 )
	{
		m_downButton->setEnabled( true );
	}
}

void DatasetListWidget::deleteItem()
{
    emit deleteSelectedItem( m_selected );

    QItemSelectionModel* selector = m_listView->selectionModel();
    QAbstractItemModel* model = m_listView->model();

    if ( model->rowCount() > 0 )
    {
        m_selected = qMin( m_selected, m_listView->model()->rowCount() - 1 );
        m_listView->selectionModel()->clearSelection();
        selector->select( model->index( m_selected, 0 ), QItemSelectionModel::Select );
        m_deleteButton->setEnabled( true );
        if ( m_selected > 0 )
        {
            m_upButton->setEnabled( true );
        }
        if ( m_selected < m_listView->model()->rowCount() - 1 )
        {
            m_downButton->setEnabled( true );
        }
    }
    else
    {
        m_listView->selectionModel()->clearSelection();
        m_selected = -1;
        m_upButton->setEnabled( false );
        m_downButton->setEnabled( false );
        m_deleteButton->setEnabled( false );
    }
}


int DatasetListWidget::getSelected()
{
    return m_selected;
}

void DatasetListWidget::slotKeyPressed( int key, Qt::KeyboardModifiers mods )
{
    if ( m_selected != -1 )
    {
        QModelIndex index = m_listView->getSelectedIndex( (int)Fn::Property::D_DATASET_POINTER );
        Dataset* ds = VPtr<Dataset>::asPtr( m_listView->model()->data( index, Qt::DisplayRole ) );

        switch ( key )
        {
//            case 65:
//            {
//                bool active = ds->properties()->get( Fn::Property::D_ACTIVE ).toBool();
//                ds->properties()->set( Fn::Property::D_ACTIVE, !active );
//                break;
//            }
            case 80:
            {
                int paintMode = ds->properties()->get( Fn::Property::D_PAINTMODE ).toInt();
                ds->properties()->set( Fn::Property::D_PAINTMODE, 1 - paintMode );
                break;
            }
        }
    }
}
