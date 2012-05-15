/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "datasetpropertyview.h"

DatasetPropertyView::DatasetPropertyView( QWidget* parent ) :
    m_currentIndex( QModelIndex() )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* layout = new QHBoxLayout();
    m_nameEdit = new QLineEdit();

    layout->addWidget( new QLabel( tr("name" ) ) );
    layout->addWidget( m_nameEdit );

    m_layout->addLayout( layout );
    m_layout->addStretch( 0 );

    m_widget->setLayout( m_layout );

    connect( m_nameEdit, SIGNAL( editingFinished() ), this, SLOT( nameEdited() ) );
}

DatasetPropertyView::~DatasetPropertyView()
{
    // TODO Auto-generated destructor stub
}

QWidget* DatasetPropertyView::getWidget()
{
    return m_widget;
}

QRect DatasetPropertyView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void DatasetPropertyView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex DatasetPropertyView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex DatasetPropertyView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int DatasetPropertyView::horizontalOffset() const
{
    return 0;
}

int DatasetPropertyView::verticalOffset() const
{
    return 0;
}

bool DatasetPropertyView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void DatasetPropertyView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion DatasetPropertyView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void DatasetPropertyView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    int sel = -1;

    if ( selected.indexes().size() > 0 )
    {
        sel = selected.indexes().first().row();
    }
    if ( sel != -1 )
    {
        m_currentIndex = model()->index( sel, 0 );
        m_nameEdit->setText( model()->data( m_currentIndex, Qt::DisplayRole ).toString() );
    }
}

void DatasetPropertyView::nameEdited()
{
    model()->setData( m_currentIndex, m_nameEdit->text() );
}
