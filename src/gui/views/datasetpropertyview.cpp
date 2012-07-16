/*
 * datasetpropertyview.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */
#include <QtGui/QtGui>

#include "../widgets/sliderwithedit.h"
#include "../widgets/selectwithlabel.h"
#include "../widgets/checkboxwithlabel.h"
#include "../../data/enums.h"

#include <QxtGui/QxtSpanSlider>

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

    m_textureInterpolation = new CheckboxWithLabel( tr("interpolation"));
    connect( m_textureInterpolation, SIGNAL( stateChanged( int ) ), this, SLOT( interpolationStateChanged( int ) ) );

    m_textureActive = new CheckboxWithLabel( tr("active") );
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

    m_spanSlider1 = new QxtSpanSlider( Qt::Horizontal );
    m_spanSlider1->setMinimum( 0 );
    m_spanSlider1->setMaximum( 100 );
    m_spanSlider1->setLowerPosition( 20 );
    m_spanSlider1->setUpperPosition( 80 );
    m_spanSlider1->setHandleMovementMode( QxtSpanSlider::NoOverlapping );
    connect( m_spanSlider1, SIGNAL( lowerPositionChanged( int ) ), this, SLOT( lower1Changed( int ) ) );
    connect( m_spanSlider1, SIGNAL( upperPositionChanged( int ) ), this, SLOT( upper1Changed( int ) ) );

    m_spanSlider2 = new QxtSpanSlider( Qt::Horizontal );
    m_spanSlider2->setMinimum( 0 );
    m_spanSlider2->setMaximum( 100 );
    m_spanSlider2->setLowerPosition( 20 );
    m_spanSlider2->setUpperPosition( 80 );
    m_spanSlider2->setHandleMovementMode( QxtSpanSlider::NoOverlapping );
    connect( m_spanSlider2, SIGNAL( lowerPositionChanged( int ) ), this, SLOT( lower2Changed( int ) ) );
    connect( m_spanSlider2, SIGNAL( upperPositionChanged( int ) ), this, SLOT( upper2Changed( int ) ) );

    m_layout->addLayout( layout1 );
    m_layout->addWidget( m_textureActive );
    m_layout->addWidget( m_lowerThresholdSlider );
    m_layout->addWidget( m_upperThresholdSlider );
    m_layout->addWidget( m_colormapSelect );
    m_layout->addWidget( m_textureInterpolation );
    m_layout->addWidget( m_alphaSlider );
    m_layout->addWidget( m_lodSelect );
    m_layout->addWidget( m_sliceSelect );
    m_layout->addWidget( m_scalingSlider );
    m_layout->addWidget( m_spanSlider1 );
    m_layout->addWidget( m_spanSlider2 );
    m_layout->addStretch( 0 );

    m_lowerThresholdSlider->setHidden( true );
    m_upperThresholdSlider->setHidden( true );
    m_colormapSelect->setHidden( true );
    m_textureInterpolation->setHidden( true );
    m_alphaSlider->setHidden( true );
    m_lodSelect->setHidden( true );
    m_sliceSelect->setHidden( true );
    m_scalingSlider->setHidden( true );
    m_spanSlider1->setHidden( true );
    m_spanSlider2->setHidden( true );

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

    QModelIndex index = getSelectedIndex( FNDSP_NAME );
    m_nameEdit->setText( model()->data( index, Qt::DisplayRole ).toString() );

    index = getSelectedIndex( FNDSP_MIN );
    m_lowerThresholdSlider->setMin( model()->data( index, Qt::DisplayRole ).toFloat() );
    m_upperThresholdSlider->setMin( model()->data( index, Qt::DisplayRole ).toFloat() );

    index = getSelectedIndex( FNDSP_MAX );
    m_lowerThresholdSlider->setMax( model()->data( index, Qt::DisplayRole ).toFloat() );
    m_upperThresholdSlider->setMax( model()->data( index, Qt::DisplayRole ).toFloat() );

    index = getSelectedIndex( FNDSE_LOWER_THRESHOLD );
    m_lowerThresholdSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );
    index = getSelectedIndex( FNDSE_UPPER_THRESHOLD );
    m_upperThresholdSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    index = getSelectedIndex( FNDSE_COLORMAP );
    m_colormapSelect->setCurrentIndex( model()->data( index, Qt::EditRole ).toInt() );

    index = getSelectedIndex( FNDSE_INTERPOLATION );
    m_textureInterpolation->setChecked( model()->data( index, Qt::EditRole ).toBool() );

    index = getSelectedIndex( FNDSE_ALPHA );
    m_alphaSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    index = getSelectedIndex( FNDSE_ACTIVE );
    m_textureActive->setChecked( model()->data( index, Qt::EditRole ).toBool() );

    index = getSelectedIndex( FNDSE_LOD );
    m_lodSelect->setCurrentIndex( model()->data( index, Qt::EditRole ).toInt() );

    m_scalingSlider->setMin( 0.1 );
    m_scalingSlider->setMax( 10 );
    index = getSelectedIndex( FNDSE_SCALING );
    m_scalingSlider->setValue( model()->data( index, Qt::EditRole ).toFloat() );

    m_sliceSelect->setCurrentIndex( model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() );

    int lowerX = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_X ), Qt::EditRole ).toInt();
    int lowerY = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_Y ), Qt::EditRole ).toInt();
    int lowerZ = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_Z ), Qt::EditRole ).toInt();
    int upperX = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_X ), Qt::EditRole ).toInt();
    int upperY = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_Y ), Qt::EditRole ).toInt();
    int upperZ = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_Z ), Qt::EditRole ).toInt();

    int maxX = model()->data( getSelectedIndex( FNDSP_NX ), Qt::DisplayRole ).toInt() - 1;
    int maxY = model()->data( getSelectedIndex( FNDSP_NY ), Qt::DisplayRole ).toInt() - 1;
    int maxZ = model()->data( getSelectedIndex( FNDSP_NZ ), Qt::DisplayRole ).toInt() - 1;

    switch ( model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() )
    {
        case 1:
            m_spanSlider1->setMaximum( maxX );
            m_spanSlider2->setMaximum( maxY );
            m_spanSlider1->setLowerPosition( lowerX );
            m_spanSlider1->setUpperPosition( upperX );
            m_spanSlider2->setLowerPosition( lowerY );
            m_spanSlider2->setUpperPosition( upperY );
            break;
        case 2:
            m_spanSlider1->setMaximum( maxX );
            m_spanSlider2->setMaximum( maxZ );
            m_spanSlider1->setLowerPosition( lowerX );
            m_spanSlider1->setUpperPosition( upperX );
            m_spanSlider2->setLowerPosition( lowerZ );
            m_spanSlider2->setUpperPosition( upperZ );
            break;
        case 3:
            m_spanSlider1->setMaximum( maxY );
            m_spanSlider2->setMaximum( maxZ );
            m_spanSlider1->setLowerPosition( lowerY );
            m_spanSlider1->setUpperPosition( upperY );
            m_spanSlider2->setLowerPosition( lowerZ );
            m_spanSlider2->setUpperPosition( upperZ );
            break;
    }
}

void DatasetPropertyView::updateWidgetVisibility()
{
    QModelIndex index = getSelectedIndex( FNDSP_DIM );
    int dim = model()->data( index, Qt::DisplayRole ).toInt();

    m_lowerThresholdSlider->setHidden( true );
    m_upperThresholdSlider->setHidden( true );
    m_colormapSelect->setHidden( true );
    m_textureInterpolation->setHidden( true );
    m_alphaSlider->setHidden( true );
    m_lodSelect->setHidden( true );
    m_sliceSelect->setHidden( true );
    m_scalingSlider->setHidden( true );
    m_spanSlider1->setHidden( true );
    m_spanSlider2->setHidden( true );

    if ( dim == 1 )
    {
        m_lowerThresholdSlider->setHidden( false );
        m_upperThresholdSlider->setHidden( false );
        m_colormapSelect->setHidden( false );
        m_textureInterpolation->setHidden( false );
        m_alphaSlider->setHidden( false );
    }

    int created = model()->data( getSelectedIndex( FNDSP_CREATED_BY ), Qt::DisplayRole ).toInt();
    if ( created == FNALGO_QBALL )
    {
        m_scalingSlider->setHidden( false );
        m_lodSelect->setHidden( false );
        m_sliceSelect->setHidden( false );

        m_spanSlider1->setHidden( false );
        m_spanSlider2->setHidden( false );
    }

    if ( dim == 999999 ) // simply for copy&paste
    {
        m_textureActive->setHidden( false );
        m_lowerThresholdSlider->setHidden( false );
        m_upperThresholdSlider->setHidden( false );
        m_colormapSelect->setHidden( false );
        m_textureInterpolation->setHidden( false );
        m_alphaSlider->setHidden( false );
        m_lodSelect->setHidden( false );
        m_sliceSelect->setHidden( false );
        m_scalingSlider->setHidden( false );
        m_spanSlider1->setHidden( false );
        m_spanSlider2->setHidden( false );
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
    model()->setData( getSelectedIndex( FNDSE_LOWER_THRESHOLD ), value );
}

void DatasetPropertyView::upperThresholdChanged( float value )
{
    if ( value < m_lowerThresholdSlider->getValue() )
    {
        m_lowerThresholdSlider->setValue( value );
    }
    model()->setData( getSelectedIndex( FNDSE_UPPER_THRESHOLD ), value );
}

void DatasetPropertyView::colormapChanged( int index )
{
    model()->setData( getSelectedIndex( FNDSE_COLORMAP ), index );
}

void DatasetPropertyView::interpolationStateChanged( int state )
{
    model()->setData( getSelectedIndex( FNDSE_INTERPOLATION ), ( state == Qt::Checked ) );
}

void DatasetPropertyView::activeStateChanged( int state )
{
    model()->setData( getSelectedIndex( FNDSE_ACTIVE ), ( state == Qt::Checked ) );
}

void DatasetPropertyView::alphaChanged( float value )
{
    model()->setData( getSelectedIndex( FNDSE_ALPHA ), value );
}

void DatasetPropertyView::lodChanged( int index )
{
    model()->setData( getSelectedIndex( FNDSE_LOD ), index );
}

void DatasetPropertyView::scalingChanged( float value )
{
    model()->setData( getSelectedIndex( FNDSE_SCALING ), value );
}

void DatasetPropertyView::renderSliceChanged( int index )
{
    model()->setData( getSelectedIndex( FNDSE_RENDER_SLICE ), index );

    int lowerX = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_X ), Qt::EditRole ).toInt();
    int lowerY = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_Y ), Qt::EditRole ).toInt();
    int lowerZ = model()->data( getSelectedIndex( FNDSE_RENDER_LOWER_Z ), Qt::EditRole ).toInt();
    int upperX = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_X ), Qt::EditRole ).toInt();
    int upperY = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_Y ), Qt::EditRole ).toInt();
    int upperZ = model()->data( getSelectedIndex( FNDSE_RENDER_UPPER_Z ), Qt::EditRole ).toInt();

    int maxX = model()->data( getSelectedIndex( FNDSP_NX ), Qt::DisplayRole ).toInt() - 1;
    int maxY = model()->data( getSelectedIndex( FNDSP_NY ), Qt::DisplayRole ).toInt() - 1;
    int maxZ = model()->data( getSelectedIndex( FNDSP_NZ ), Qt::DisplayRole ).toInt() - 1;


    switch ( index )
    {
        case 1:
            m_spanSlider1->setMaximum( maxX );
            m_spanSlider2->setMaximum( maxY );
            m_spanSlider1->setLowerPosition( lowerX );
            m_spanSlider1->setUpperPosition( upperX );
            m_spanSlider2->setLowerPosition( lowerY );
            m_spanSlider2->setUpperPosition( upperY );
            break;
        case 2:
            m_spanSlider1->setMaximum( maxX );
            m_spanSlider2->setMaximum( maxZ );
            m_spanSlider1->setLowerPosition( lowerX );
            m_spanSlider1->setUpperPosition( upperX );
            m_spanSlider2->setLowerPosition( lowerZ );
            m_spanSlider2->setUpperPosition( upperZ );
            break;
        case 3:
            m_spanSlider1->setMaximum( maxY );
            m_spanSlider2->setMaximum( maxZ );
            m_spanSlider1->setLowerPosition( lowerY );
            m_spanSlider1->setUpperPosition( upperY );
            m_spanSlider2->setLowerPosition( lowerZ );
            m_spanSlider2->setUpperPosition( upperZ );
            break;
    }
}

void DatasetPropertyView::lower1Changed( int value )
{
    switch (  model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() )
    {
        case 1:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_X ), value );
            break;
        case 2:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_X ), value );
            break;
        case 3:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_Y ), value );
            break;
    }
}

void DatasetPropertyView::upper1Changed( int value )
{
    switch (  model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() )
    {
        case 1:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_X ), value );
            break;
        case 2:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_X ), value );
            break;
        case 3:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_Y ), value );
            break;
    }
}

void DatasetPropertyView::lower2Changed( int value )
{
    switch (  model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() )
    {
        case 1:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_Y ), value );
            break;
        case 2:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_Z ), value );
            break;
        case 3:
            model()->setData( getSelectedIndex( FNDSE_RENDER_LOWER_Z ), value );
            break;
    }
}

void DatasetPropertyView::upper2Changed( int value )
{
    switch (  model()->data( getSelectedIndex( FNDSE_RENDER_SLICE ), Qt::EditRole ).toInt() )
    {
        case 1:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_Y ), value );
            break;
        case 2:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_Z ), value );
            break;
        case 3:
            model()->setData( getSelectedIndex( FNDSE_RENDER_UPPER_Z ), value );
            break;
    }
}

