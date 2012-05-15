/*
 * globalpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "globalpropertyview.h"

GlobalPropertyView::GlobalPropertyView( QWidget* parent )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* layout1 = new QHBoxLayout();
    m_sagittalSlider = new QSlider();
    m_sagittalSlider->setOrientation( Qt::Horizontal );
    m_sagittalEdit = new QLineEdit();
    m_sagittalEdit->setMaxLength( 3 );
    m_sagittalEdit->setMaximumWidth( 30 );
    connect( m_sagittalSlider, SIGNAL( valueChanged( int) ), this, SLOT( sagittalSliderChanged( int ) ) );
    connect( m_sagittalEdit, SIGNAL( textEdited( QString) ), this, SLOT( sagittalEditChanged( QString ) ) );
    layout1->addWidget( new QLabel( tr("sagittal" ) ) );
    layout1->addWidget( m_sagittalSlider );
    layout1->addWidget( m_sagittalEdit );
    m_layout->addLayout( layout1 );

    QHBoxLayout* layout2 = new QHBoxLayout();
    m_coronalSlider = new QSlider();
    m_coronalSlider->setOrientation( Qt::Horizontal );
    m_coronalEdit = new QLineEdit();
    m_coronalEdit->setMaxLength( 3 );
    m_coronalEdit->setMaximumWidth( 30 );
    connect( m_coronalSlider, SIGNAL( valueChanged( int) ), this, SLOT( coronalSliderChanged( int ) ) );
    connect( m_coronalEdit, SIGNAL( textEdited( QString) ), this, SLOT( coronalEditChanged( QString ) ) );
    layout2->addWidget( new QLabel( tr("coronal" ) ) );
    layout2->addWidget( m_coronalSlider );
    layout2->addWidget( m_coronalEdit );
    m_layout->addLayout( layout2 );

    QHBoxLayout* layout3 = new QHBoxLayout();
    m_axialSlider = new QSlider();
    m_axialSlider->setOrientation( Qt::Horizontal );
    m_axialEdit = new QLineEdit();
    m_axialEdit->setMaxLength( 3 );
    m_axialEdit->setMaximumWidth( 30 );
    connect( m_axialSlider, SIGNAL( valueChanged( int) ), this, SLOT( axialSliderChanged( int ) ) );
    connect( m_axialEdit, SIGNAL( textEdited( QString) ), this, SLOT( axialEditChanged( QString ) ) );
    layout3->addWidget( new QLabel( tr("axial" ) ) );
    layout3->addWidget( m_axialSlider );
    layout3->addWidget( m_axialEdit );
    m_layout->addLayout( layout3 );


    m_layout->addStretch( 0 );
    m_widget->setLayout( m_layout );
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
        m_sagittalSlider->setMaximum( model()->data( mi, Qt::UserRole ).toInt() );
    }

    mi = model()->index( 0, 101 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, 104 );
    if ( mi.isValid() )
    {
        m_coronalSlider->setMaximum( model()->data( mi, Qt::UserRole ).toInt() );
    }

    mi = model()->index( 0, 102 );
    if ( mi.isValid() )
    {
        m_axialSlider->setValue( model()->data( mi, Qt::UserRole ).toInt() );
    }
    mi = model()->index( 0, 105 );
    if ( mi.isValid() )
    {
        m_axialSlider->setMaximum( model()->data( mi, Qt::UserRole ).toInt() );
    }
}

void GlobalPropertyView::sagittalSliderChanged( int value )
{
    m_sagittalEdit->setText( QString::number( value, 10 ) );
    QModelIndex mi;

    mi = model()->index( 0, 100 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}

void GlobalPropertyView::coronalSliderChanged( int value )
{
    m_coronalEdit->setText( QString::number( value, 10 ) );
    QModelIndex mi;

    mi = model()->index( 0, 101 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}

void GlobalPropertyView::axialSliderChanged( int value )
{
    m_axialEdit->setText( QString::number( value, 10 ) );
    QModelIndex mi;

    mi = model()->index( 0, 102 );
    if ( mi.isValid() )
    {
        model()->setData( mi, value, Qt::UserRole );
    }
}

void GlobalPropertyView::sagittalEditChanged( QString text )
{
    m_sagittalSlider->setValue( text.toInt() );
}

void GlobalPropertyView::coronalEditChanged( QString text )
{
    m_coronalSlider->setValue( text.toInt() );
}

void GlobalPropertyView::axialEditChanged( QString text )
{
    m_axialSlider->setValue( text.toInt() );
}
