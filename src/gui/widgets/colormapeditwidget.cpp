/*
 * colormapeditwidget.cpp
 *
 *  Created on: 12.02.2013
 *      Author: Ralph Schurade
 */
#include "colormapeditwidget.h"

#include "../widgets/controls/checkboxwithlabel.h"
#include "../widgets/controls/colorwidgetwithlabel.h"
#include "../widgets/controls/editwithlabel.h"
#include "../widgets/controls/pushbuttonwithid.h"
#include "../widgets/controls/selectwithlabel.h"
#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/sliderwitheditint.h"

#include "../gl/glfunctions.h"
#include "../gl/colormapfunctions.h"

#include "../../data/enums.h"
#include "../../data/properties/property.h"

#include <QtGui>

ColormapEditWidget::ColormapEditWidget( QWidget* parent ) :
    QWidget( parent ),
    m_selected( 0 ),
    m_contUpdating( false )
{
    m_colormap = ColormapFunctions::get( 0 );
    redrawWidget();

    setContentsMargins( 0, 0, 0, 0 );

    this->setMaximumWidth( 500 );
    setStyleSheet( "QLabel { font:  bold 12px } "
                     "QPushButton { font:  bold 12px; max-height: 16px; } ");
}

ColormapEditWidget::~ColormapEditWidget()
{
}

void ColormapEditWidget::resizeEvent( QResizeEvent* event )
{
    QImage* image;
    image = createImage( this->width() - 20 );
    QPixmap pix( this->width() - 20, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );
}

void ColormapEditWidget::redrawWidget()
{
    if( layout() )
    {
        QWidget().setLayout(layout());
    }

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setSpacing( 1 );
    setLayout( vLayout );

    QHBoxLayout* hLayoutTop = new QHBoxLayout();
    m_nameEdit = new EditWithLabel( "name" );
    m_nameEdit->setText( ColormapFunctions::get( m_selected).getName() );

    m_sel = new SelectWithLabel( "colormap", 0 );
    for ( int k = 0; k < ColormapFunctions::size(); ++k )
    {
         m_sel->insertItem( k, ColormapFunctions::get( k ).getName() );

    }
    m_sel->setCurrentIndex( m_selected );
    connect( m_sel, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( selectionChanged( int) ) );

    hLayoutTop->addWidget( m_nameEdit );
    hLayoutTop->addWidget( m_sel );
    vLayout->addLayout( hLayoutTop );

    m_buttonPanel = new QWidget();
    m_buttonPanel->setLayout( createButtonPanel() );
    vLayout->addWidget( m_buttonPanel );

    vLayout->addStretch();
    QHBoxLayout* hLayout5 = new QHBoxLayout();
    hLayout5->addStretch();
    CheckboxWithLabel* contUpdating = new CheckboxWithLabel( tr("continuous updating"), false );
    contUpdating->setChecked( m_contUpdating );
    connect( contUpdating, SIGNAL( stateChanged( int, int ) ), this, SLOT( contUpdatingChanged( int ) ) );
    hLayout5->addWidget( contUpdating );
    QPushButton* updateButton = new QPushButton( tr("update") );
    connect( updateButton, SIGNAL( clicked() ), this, SLOT( update() ) );
    hLayout5->addWidget( updateButton );
    QPushButton* saveButton = new QPushButton( tr("save new") );
    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    hLayout5->addWidget( saveButton );
    vLayout->addLayout( hLayout5 );

    layout()->setContentsMargins( 1, 1, 1, 1 );
    layout()->setSpacing( 1 );

    repaint();
}

void ColormapEditWidget::updateWidget()
{
    if( m_buttonPanel->layout() )
    {
        QWidget().setLayout( m_buttonPanel->layout() );
    }
    m_buttonPanel->setLayout( createButtonPanel() );
}

QVBoxLayout* ColormapEditWidget::createButtonPanel()
{
    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_cLabel = new QLabel( this );

    QImage* image;
    image = createImage( this->width() - 20 );
    QPixmap pix( this->width() - 20, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );
    hLayout->addStretch();
    hLayout->addWidget( m_cLabel );
    hLayout->addStretch();
    vLayout->addLayout( hLayout );
    vLayout->addSpacing( 5 );

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addStretch();
    ColorWidgetWithLabel* colorWidget = new ColorWidgetWithLabel( tr("0"), 0 );
    colorWidget->setValue( m_colormap.get( 0 ).color );
    connect( colorWidget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );
    hLayout1->addWidget( colorWidget );
    vLayout->addLayout( hLayout1 );
    m_sliders.clear();
    for ( int i = 1; i < m_colormap.size() - 1; ++i )
    {
        QHBoxLayout* hLayout4 = new QHBoxLayout();
        hLayout4->addStretch();
        PushButtonWithId* newButton= new PushButtonWithId( tr("new entry"), i );
        connect( newButton, SIGNAL( signalClicked(int ) ), this, SLOT( newEntry( int ) ) );
        hLayout4->addWidget( newButton );
        hLayout4->addStretch();
        vLayout->addLayout( hLayout4 );

        QHBoxLayout* hLayout2 = new QHBoxLayout();
        SliderWithEdit* slider = new SliderWithEdit( tr(""), i );
        m_sliders.push_back( slider );
        slider->setValue( m_colormap.get( i ).value );
        slider->setMin( m_colormap.get( i - 1 ).value );
        slider->setMax( m_colormap.get( i + 1 ).value );
        connect( slider, SIGNAL( valueChanged( float, int ) ), this, SLOT( sliderChanged( float, int ) ) );
        ColorWidgetWithLabel* colorWidget = new ColorWidgetWithLabel( QString::number( i ), i );
        colorWidget->setValue( m_colormap.get( i ).color );
        connect( colorWidget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );
        PushButtonWithId* removeButton = new PushButtonWithId( tr("remove"), i );
        connect( removeButton, SIGNAL( signalClicked( int) ), this, SLOT( removeEntry( int ) ) );

        hLayout2->addWidget( slider );
        hLayout2->addWidget( colorWidget );
        hLayout2->addWidget( removeButton );
        vLayout->addLayout( hLayout2 );
    }

    QHBoxLayout* hLayout4 = new QHBoxLayout();
    hLayout4->addStretch();
    PushButtonWithId* newButton= new PushButtonWithId( tr("new entry"), m_colormap.size() - 1 );
    connect( newButton, SIGNAL( signalClicked(int ) ), this, SLOT( newEntry( int ) ) );
    hLayout4->addWidget( newButton );
    hLayout4->addStretch();
    vLayout->addLayout( hLayout4 );

    QHBoxLayout* hLayout3 = new QHBoxLayout();
    hLayout3->addStretch();
    ColorWidgetWithLabel* colorWidget2 = new ColorWidgetWithLabel( QString::number( m_colormap.size() - 1 ), m_colormap.size() - 1 );
    colorWidget2->setValue( m_colormap.get( m_colormap.size() - 1 ).color );
    connect( colorWidget2, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );
    hLayout3->addWidget( colorWidget2 );
    vLayout->addLayout( hLayout3 );

    return vLayout;
}

QImage* ColormapEditWidget::createImage( int width )
{
    QImage* image = new QImage( width, 20, QImage::Format_RGB32 );
    for ( int i = 0; i < width; ++i )
    {
        QColor c;

        c = m_colormap.getColor( (float)i / (float)width );

        for ( int k = 0; k < 20; ++k )
        {
            image->setPixel( i, k, c.rgba() );
        }
    }
    return image;
}

void ColormapEditWidget::sliderChanged( float value, int id )
{
    m_colormap.setValue( id, value );

    QImage* image;
    image = createImage( this->width() - 20 );
    QPixmap pix( this->width() - 20, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );

    for ( int i = 0; i < m_sliders.size(); ++i )
    {
        m_sliders[i]->setMin( m_colormap.get( i ).value);
        m_sliders[i]->setMax( m_colormap.get( i + 2 ).value );
    }
    repaint();

    if ( m_contUpdating )
    {
        update();
    }
}

void ColormapEditWidget::colorChanged( QColor color, int id )
{
    m_colormap.setColor( id, color );

    QImage* image;
    image = createImage( this->width() - 20 );
    QPixmap pix( this->width() - 20, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );

    if ( m_contUpdating )
    {
        update();
    }
}

void ColormapEditWidget::newEntry( int id )
{
    float v1 = m_colormap.get( id - 1 ).value;
    float v2 = m_colormap.get( id ).value;
    float value = v1 + ( ( v2 - v1 ) / 2.0 );
    m_colormap.insertValue( value, QColor( 255, 255, 255 ) );
    redrawWidget();
    if ( m_contUpdating )
    {
        update();
    }
}

void ColormapEditWidget::removeEntry( int id )
{
    m_colormap.removeValue( id );
    redrawWidget();
    if ( m_contUpdating )
    {
        update();
    }
}

void ColormapEditWidget::save()
{
    m_colormap.setName( m_nameEdit->text() );
    ColormapFunctions::addColormap( m_colormap );
    GLFunctions::updateColormapShader();
    m_selected = ColormapFunctions::size() - 1;
    redrawWidget();

    emit( signalUpdate() );
}

void ColormapEditWidget::update()
{
    m_colormap.setName( m_nameEdit->text() );
    ColormapFunctions::updateColormap( m_selected, m_colormap );
    GLFunctions::updateColormapShader();

    emit( signalUpdate() );
}

void ColormapEditWidget::selectionChanged( int id )
{
    m_colormap = ColormapFunctions::get( id );
    m_nameEdit->setText( m_colormap.getName() );
    m_selected = id;

    updateWidget();
}

void ColormapEditWidget::contUpdatingChanged( int value )
{
    m_contUpdating = value;
}
