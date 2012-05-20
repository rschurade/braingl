/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "widgets/sliderwithedit.h"

#include "datasetpropertyview.h"

DatasetPropertyView::DatasetPropertyView( QWidget* parent ) :
    m_selected( QItemSelection() )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* layout1 = new QHBoxLayout();
    m_nameEdit = new QLineEdit();
    layout1->addWidget( new QLabel( tr("name" ) ) );
    layout1->addWidget( m_nameEdit );
    connect( m_nameEdit, SIGNAL( editingFinished() ), this, SLOT( nameEdited() ) );

    m_lowerThresholdSlider = new SliderWithEdit( tr( "lower threshold" ) );
    connect( m_lowerThresholdSlider, SIGNAL( valueChanged( float ) ), this, SLOT( lowerThresholdChanged( float ) ) );

    m_upperThresholdSlider = new SliderWithEdit( tr( "upper threshold" ) );
    connect( m_upperThresholdSlider, SIGNAL( valueChanged( float ) ), this, SLOT( upperThresholdChanged( float ) ) );

    QHBoxLayout* layout2 = new QHBoxLayout();
    m_colormapSelect = new QComboBox();
    m_colormapSelect->insertItem( 0, tr( "grey" ) );
    m_colormapSelect->insertItem( 1, tr( "rainbow" ) );
    m_colormapSelect->insertItem( 2, tr( "unused" ) );
    layout2->addWidget( new QLabel( tr("colormap" ) ) );
    layout2->addWidget( m_colormapSelect );
    connect( m_colormapSelect, SIGNAL( currentIndexChanged( int) ), this, SLOT( colormapChanged( int ) ) );

    QHBoxLayout* layout3 = new QHBoxLayout();
    m_textureInterpolation = new QCheckBox();
    layout3->addWidget( new QLabel( tr("interpolation" ) ) );
    layout3->addWidget( m_textureInterpolation );
    connect( m_textureInterpolation, SIGNAL( stateChanged( int ) ), this, SLOT( interpolationStateChanged( int ) ) );

    QHBoxLayout* layout4 = new QHBoxLayout();
    m_textureActive = new QCheckBox();
    layout4->addWidget( new QLabel( tr("active" ) ) );
    layout4->addWidget( m_textureActive );
    connect( m_textureActive, SIGNAL( stateChanged( int ) ), this, SLOT( activeStateChanged( int ) ) );

    m_alphaSlider = new SliderWithEdit( tr( "alpha" ) );
    connect( m_alphaSlider, SIGNAL( valueChanged( float ) ), this, SLOT( alphaChanged( float ) ) );
    m_alphaSlider->setMin( 0.0 );
    m_alphaSlider->setMax( 1.0 );

    m_layout->addLayout( layout1 );
    m_layout->addLayout( layout4 );
    m_layout->addLayout( m_lowerThresholdSlider );
    m_layout->addLayout( m_upperThresholdSlider );
    m_layout->addLayout( layout2 );
    m_layout->addLayout( layout3 );
    m_layout->addLayout( m_alphaSlider );
    m_layout->addStretch( 0 );

    m_widget->setLayout( m_layout );


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

QModelIndex DatasetPropertyView::getSelectedIndex( int column )
{
    if ( m_selected.indexes().size() > 0 )
    {
        int sel = -1;
        sel = m_selected.indexes().first().row();
        QModelIndex index = model()->index( sel, column );
        return index;
    }
    return QModelIndex();
}

void DatasetPropertyView::selectionChanged( const QItemSelection &selected, const QItemSelection &deselected )
{
    m_selected = selected;

    QModelIndex index = getSelectedIndex( 0 );
    m_nameEdit->setText( model()->data( index, Qt::DisplayRole ).toString() );

    index = getSelectedIndex( 10 );
    m_lowerThresholdSlider->setMin( model()->data( index, Qt::DisplayRole ).toFloat() );
    m_upperThresholdSlider->setMin( model()->data( index, Qt::DisplayRole ).toFloat() );

    index = getSelectedIndex( 11 );
    m_lowerThresholdSlider->setMax( model()->data( index, Qt::DisplayRole ).toFloat() );
    m_upperThresholdSlider->setMax( model()->data( index, Qt::DisplayRole ).toFloat() );

    index = getSelectedIndex( 50 );
    m_lowerThresholdSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );
    index = getSelectedIndex( 51 );
    m_upperThresholdSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    index = getSelectedIndex( 52 );
    m_colormapSelect->setCurrentIndex( model()->data( index, Qt::EditRole ).toInt() );

    index = getSelectedIndex( 53 );
    m_textureInterpolation->setChecked( model()->data( index, Qt::EditRole ).toBool() );

    index = getSelectedIndex( 54 );
    m_alphaSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    index = getSelectedIndex( 55 );
    m_textureActive->setChecked( model()->data( index, Qt::EditRole ).toBool() );
}

void DatasetPropertyView::nameEdited()
{
    model()->setData( getSelectedIndex( 0 ), m_nameEdit->text() );
}

void DatasetPropertyView::lowerThresholdChanged( float value )
{
    if ( value > m_upperThresholdSlider->getValue() )
    {
        m_upperThresholdSlider->setValue( value );
    }
    model()->setData( getSelectedIndex( 50 ), value );
}

void DatasetPropertyView::upperThresholdChanged( float value )
{
    if ( value < m_lowerThresholdSlider->getValue() )
    {
        m_lowerThresholdSlider->setValue( value );
    }
    model()->setData( getSelectedIndex( 51 ), value );
}

void DatasetPropertyView::colormapChanged( int index )
{
    model()->setData( getSelectedIndex( 52 ), index );
}

void DatasetPropertyView::interpolationStateChanged( int state )
{
    if ( state == Qt::Checked )
    {
        model()->setData( getSelectedIndex( 53 ), true );
    }
    else
    {
        model()->setData( getSelectedIndex( 53 ), false );
    }
}

void DatasetPropertyView::activeStateChanged( int state )
{
    if ( state == Qt::Checked )
    {
        model()->setData( getSelectedIndex( 55 ), true );
    }
    else
    {
        model()->setData( getSelectedIndex( 55 ), false );
    }
}

void DatasetPropertyView::alphaChanged( float value )
{
    model()->setData( getSelectedIndex( 54 ), value );
}
