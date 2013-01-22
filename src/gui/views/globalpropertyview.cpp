/*
 * globalpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../widgets/sliderwitheditint.h"
#include "../../data/enums.h"
#include "globalpropertyview.h"

GlobalPropertyView::GlobalPropertyView( QWidget* parent )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    m_sagittalSlider = new SliderWithEditInt( "sagittal", FNGLOBAL_SAGITTAL );
    m_coronalSlider = new SliderWithEditInt( "coronal", FNGLOBAL_CORONAL );
    m_axialSlider = new SliderWithEditInt( "axial", FNGLOBAL_AXIAL );

    m_layout->addWidget( m_sagittalSlider );
    m_layout->addWidget( m_coronalSlider );
    m_layout->addWidget( m_axialSlider );

    m_layout->addStretch( 0 );
    m_widget->setLayout( m_layout );

    connect( m_sagittalSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
    connect( m_coronalSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
    connect( m_axialSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( sliderChanged( int, int ) ) );
}

GlobalPropertyView::~GlobalPropertyView()
{
    // TODO Auto-generated destructor stub
}

void GlobalPropertyView::init()
{
}

QWidget* GlobalPropertyView::getWidget()
{
    return m_widget;
}

QRect GlobalPropertyView::visualRect( const QModelIndex &index ) const
{
    return QRect( 0, 0, 0, 0 );
}

void GlobalPropertyView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex GlobalPropertyView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex GlobalPropertyView::moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

int GlobalPropertyView::horizontalOffset() const
{
    return 0;
}

int GlobalPropertyView::verticalOffset() const
{
    return 0;
}

bool GlobalPropertyView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void GlobalPropertyView::setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags )
{
}

QRegion GlobalPropertyView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void GlobalPropertyView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    QModelIndex mi;
    mi = model()->index( 0, FNGLOBAL_SAGITTAL );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, FNGLOBAL_MAX_SAGITTAL );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() - 1 );
    }

    mi = model()->index( 0, FNGLOBAL_CORONAL );
    if ( mi.isValid() )
    {
        m_coronalSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, FNGLOBAL_MAX_CORONAL );
    if ( mi.isValid() )
    {
        m_coronalSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() - 1 );
    }

    mi = model()->index( 0, FNGLOBAL_AXIAL );
    if ( mi.isValid() )
    {
        m_axialSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, FNGLOBAL_MAX_AXIAL );
    if ( mi.isValid() )
    {
        m_axialSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() - 1 );
    }
}

void GlobalPropertyView::sliderChanged( int value, int id )
{
    model()->setData( model()->index( 0, id ), value, Qt::UserRole );
}
