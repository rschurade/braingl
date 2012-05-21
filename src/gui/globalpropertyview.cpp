/*
 * globalpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "widgets/sliderwitheditint.h"

#include "globalpropertyview.h"

GlobalPropertyView::GlobalPropertyView( QWidget* parent )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    m_sagittalSlider = new SliderWithEditInt( "sagittal" );
    m_coronalSlider = new SliderWithEditInt( "coronal" );
    m_axialSlider = new SliderWithEditInt( "axial" );

    m_layout->addLayout( m_sagittalSlider );
    m_layout->addLayout( m_coronalSlider );
    m_layout->addLayout( m_axialSlider );

    m_layout->addStretch( 0 );
    m_widget->setLayout( m_layout );

    connect( m_sagittalSlider, SIGNAL( valueChanged( int ) ), this, SLOT( sagittalSliderChanged( int ) ) );
    connect( m_coronalSlider, SIGNAL( valueChanged( int ) ), this, SLOT( coronalSliderChanged( int ) ) );
    connect( m_axialSlider, SIGNAL( valueChanged( int ) ), this, SLOT( axialSliderChanged( int ) ) );
}

GlobalPropertyView::~GlobalPropertyView()
{
    // TODO Auto-generated destructor stub
}

void GlobalPropertyView::init()
{
    QModelIndex mi;
    mi = model()->index( 0, 99 );
    if ( mi.isValid() )
    {
        model()->setData( mi, true, Qt::UserRole );
    }
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
    mi = model()->index( 0, 100 );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, 103 );
    if ( mi.isValid() )
    {
        m_sagittalSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() );
    }

    mi = model()->index( 0, 101 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, 104 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() );
    }

    mi = model()->index( 0, 102 );
    if ( mi.isValid() )
    {
        m_axialSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, 105 );
    if ( mi.isValid() )
    {
        m_axialSlider->setMax( model()->data( mi, Qt::UserRole ).toInt() );
    }
}

void GlobalPropertyView::sagittalSliderChanged( int value )
{
    QModelIndex mi;

    mi = model()->index( 0, 100 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}

void GlobalPropertyView::coronalSliderChanged( int value )
{
    QModelIndex mi;

    mi = model()->index( 0, 101 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}

void GlobalPropertyView::axialSliderChanged( int value )
{
    QModelIndex mi;

    mi = model()->index( 0, 102 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}
