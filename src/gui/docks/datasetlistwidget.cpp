/*
 * datasetviewwidget.cpp
 *
 *  Created on: 08.05.2012
 *      Author: Ralph
 */
#include <string>
#include <iostream>

#include <QtGui/QtGui>

#include "../views/datasetlistview.h"

#include "datasetlistwidget.h"

DatasetListWidget::DatasetListWidget( QWidget* parent ) :
	QDockWidget( "Datasets", parent ),
	m_selected( -1 )
{
	setObjectName( "Dataset View Dock" );

	m_listView = new DatasetListView( this );
	m_listView->setToolTip( QString( "datasets" ) );

	this->setAllowedAreas( Qt::AllDockWidgetAreas );
	this->setFeatures( QDockWidget::AllDockWidgetFeatures );

	QWidget* panel = new QWidget( this );

	m_layout = new QVBoxLayout();

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	m_downButton = new QPushButton();
	m_downButton->setText( QString( "down" ) );
	m_upButton = new QPushButton();
	m_upButton->setText( QString( "up" ) );
	m_deleteButton = new QPushButton();
	m_deleteButton->setText( QString( "delete" ) );

	buttonLayout->addWidget( m_downButton );
	buttonLayout->addWidget( m_upButton );
	buttonLayout->addWidget( m_deleteButton );

	connect( m_upButton, SIGNAL( pressed() ), this, SLOT( moveItemUp() ) );
	connect( m_downButton, SIGNAL( pressed() ), this, SLOT( moveItemDown() ) );
	connect( m_deleteButton, SIGNAL( pressed() ), this, SLOT( deleteItem() ) );

	connect( m_listView, SIGNAL(itemSelectionChanged( const QItemSelection ) ), this, SLOT( itemSelectionChanged( const QItemSelection ) ) );

	m_layout->addWidget( m_listView );
	m_layout->addLayout( buttonLayout );

	panel->setLayout( m_layout );
	setWidget( panel );

	m_upButton->setEnabled( false );
	m_downButton->setEnabled( false );
	m_deleteButton->setEnabled( false );
}

DatasetListWidget::~DatasetListWidget()
{
}

void DatasetListWidget::setModel (QAbstractItemModel  *model )
{
	m_listView->setModel( model );
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
