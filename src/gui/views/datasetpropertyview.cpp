/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../widgets/sliderwithedit.h"
#include "../widgets/SelectWithLabel.h"
#include "../../data/enums.h"

#include "datasetpropertyview.h"


DatasetPropertyView::DatasetPropertyView( QWidget* parent ) :
    m_selected( QItemSelection() )
{
    m_widget = new QWidget( this );

    m_layout = new QVBoxLayout();

    QHBoxLayout* layout1 = new QHBoxLayout();
    m_nameEdit = new QLineEdit();
    layout1->addWidget( new QLabel( tr("name" ) ), 25 );
    layout1->addWidget( m_nameEdit, 75 );
    connect( m_nameEdit, SIGNAL( editingFinished() ), this, SLOT( nameEdited() ) );

    m_lowerThresholdSlider = new SliderWithEdit( tr( "lower threshold" ) );
    connect( m_lowerThresholdSlider, SIGNAL( valueChanged( float ) ), this, SLOT( lowerThresholdChanged( float ) ) );

    m_upperThresholdSlider = new SliderWithEdit( tr( "upper threshold" ) );
    connect( m_upperThresholdSlider, SIGNAL( valueChanged( float ) ), this, SLOT( upperThresholdChanged( float ) ) );

    m_colormapSelect = new SelectWithLabel( tr("colormap" ) );
    m_colormapSelect->insertItem( 0, tr( "grey" ) );
    m_colormapSelect->insertItem( 1, tr( "rainbow" ) );
    m_colormapSelect->insertItem( 2, tr( "unused" ) );
    connect( m_colormapSelect, SIGNAL( currentIndexChanged( int) ), this, SLOT( colormapChanged( int ) ) );

    QHBoxLayout* layout3 = new QHBoxLayout();
    m_textureInterpolation = new QCheckBox();
    layout3->addWidget( new QLabel( tr("interpolation" ) ), 25 );
    layout3->addWidget( m_textureInterpolation, 75 );
    connect( m_textureInterpolation, SIGNAL( stateChanged( int ) ), this, SLOT( interpolationStateChanged( int ) ) );

    QHBoxLayout* layout4 = new QHBoxLayout();
    m_textureActive = new QCheckBox();
    layout4->addWidget( new QLabel( tr("active" ) ), 25 );
    layout4->addWidget( m_textureActive, 75 );
    connect( m_textureActive, SIGNAL( stateChanged( int ) ), this, SLOT( activeStateChanged( int ) ) );

    m_alphaSlider = new SliderWithEdit( tr( "alpha" ) );
    connect( m_alphaSlider, SIGNAL( valueChanged( float ) ), this, SLOT( alphaChanged( float ) ) );
    m_alphaSlider->setMin( 0.0 );
    m_alphaSlider->setMax( 1.0 );

    m_lodSelect = new SelectWithLabel( tr("level of detail" ) );
    m_lodSelect->insertItem( 0, tr( "0" ) );
    m_lodSelect->insertItem( 1, tr( "1" ) );
    m_lodSelect->insertItem( 2, tr( "2" ) );
    m_lodSelect->insertItem( 3, tr( "3" ) );
    m_lodSelect->insertItem( 4, tr( "4" ) );
    m_lodSelect->insertItem( 5, tr( "5" ) );
    connect( m_lodSelect, SIGNAL( currentIndexChanged( int ) ), this, SLOT( lodChanged( int ) ) );

    m_sliceSelect = new SelectWithLabel( tr("render on slice" ) );
    m_sliceSelect->insertItem( 0, tr( "none" ) );
    m_sliceSelect->insertItem( 1, tr( "axial" ) );
    m_sliceSelect->insertItem( 2, tr( "coronal" ) );
    m_sliceSelect->insertItem( 3, tr( "sagittal" ) );
    connect( m_sliceSelect, SIGNAL( currentIndexChanged( int ) ), this, SLOT( renderSliceChanged( int ) ) );

    m_scalingSlider = new SliderWithEdit( tr( "qball scaling" ) );
    connect( m_scalingSlider, SIGNAL( valueChanged( float ) ), this, SLOT( scalingChanged( float ) ) );

    m_layout->addLayout( layout1 );
    m_layout->addLayout( layout4 );
    m_layout->addWidget( m_lowerThresholdSlider );
    m_layout->addWidget( m_upperThresholdSlider );
    m_layout->addWidget( m_colormapSelect );
    m_layout->addLayout( layout3 );
    m_layout->addWidget( m_alphaSlider );
    m_layout->addWidget( m_lodSelect );
    m_layout->addWidget( m_sliceSelect );
    m_layout->addWidget( m_scalingSlider );
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

    updateWidgetVisibility();

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

    index = getSelectedIndex( 56 );
    m_lodSelect->setCurrentIndex( model()->data( index, Qt::EditRole ).toInt() );

    m_scalingSlider->setMin( 0.1 );
    m_scalingSlider->setMax( 10 );
    index = getSelectedIndex( 57 );
    m_scalingSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    index = getSelectedIndex( 58 );
    m_sliceSelect->setCurrentIndex( model()->data( index, Qt::EditRole ).toInt() );
}

void DatasetPropertyView::updateWidgetVisibility()
{
    QModelIndex index = getSelectedIndex( 1 );
    int dim = model()->data( index, Qt::DisplayRole ).toInt();

    m_lowerThresholdSlider->setHidden( true );
    m_upperThresholdSlider->setHidden( true );

    m_scalingSlider->setHidden( true );
    m_lodSelect->setHidden( true );
    m_sliceSelect->setHidden( true );


    if ( dim == 1 )
    {
        m_lowerThresholdSlider->setHidden( false );
        m_upperThresholdSlider->setHidden( false );
    }

    int created = model()->data( getSelectedIndex( 13 ), Qt::DisplayRole ).toInt();
    if ( created == FNALGO_QBALL )
    {
        m_scalingSlider->setHidden( false );
        m_lodSelect->setHidden( false );
        m_sliceSelect->setHidden( false );
    }
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

void DatasetPropertyView::lodChanged( int index )
{
    model()->setData( getSelectedIndex( 56 ), index );
}

void DatasetPropertyView::scalingChanged( float value )
{
    model()->setData( getSelectedIndex( 57 ), value );
}

void DatasetPropertyView::renderSliceChanged( int index )
{
    model()->setData( getSelectedIndex( 58 ), index );
}
