/*
 * colormapeditwidget.cpp
 *
 *  Created on: 12.02.2013
 *      Author: Ralph Schurade
 */
#include "colormapeditwidget.h"

#include "../widgets/controls/sliderwithedit.h"
#include "../widgets/controls/colorwidgetwithlabel.h"
#include "../widgets/controls/pushbuttonwithid.h"

#include "../gl/glfunctions.h"

#include "../../data/enums.h"
#include "../../data/properties/property.h"

#include <QtGui/QtGui>

ColormapEditWidget::ColormapEditWidget( QWidget* parent ) :
    QWidget( parent ),
    m_colormap( QColor( 0, 0, 255), QColor( 255, 0, 0 ) )
{
    redrawWidget();

    setContentsMargins( 0, 0, 0, 0 );

    this->setMaximumWidth( 500 );
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
        QHBoxLayout* hLayout2 = new QHBoxLayout();
        SliderWithEdit* slider = new SliderWithEdit( tr(""), i );
        m_sliders.push_back( slider );
        slider->setValue( m_colormap.get( i ).value );
        slider->setMin( m_colormap.get( i - 1 ).value + 0.01 );
        slider->setMax( m_colormap.get( i + 1 ).value - 0.01 );
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
    m_newButton = new QPushButton( tr("new entry") );
    connect( m_newButton, SIGNAL( clicked() ), this, SLOT( newEntry() ) );
    hLayout4->addWidget( m_newButton );
    hLayout4->addStretch();
    vLayout->addLayout( hLayout4 );

    QHBoxLayout* hLayout3 = new QHBoxLayout();
    hLayout3->addStretch();
    ColorWidgetWithLabel* colorWidget2 = new ColorWidgetWithLabel( QString::number( m_colormap.size() - 1 ), m_colormap.size() - 1 );
    colorWidget2->setValue( m_colormap.get( m_colormap.size() - 1 ).color );
    connect( colorWidget2, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );
    hLayout3->addWidget( colorWidget2 );
    vLayout->addLayout( hLayout3 );

    QHBoxLayout* hLayout5 = new QHBoxLayout();
    hLayout5->addStretch();
    QPushButton* saveButton = new QPushButton( tr("save") );
    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    hLayout5->addWidget( saveButton );
    vLayout->addLayout( hLayout5 );

    layout()->setContentsMargins( 1, 1, 1, 1 );
    layout()->setSpacing( 1 );

    vLayout->addStretch();
    repaint();
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
    image = createImage( this->width() - 6 );
    QPixmap pix( this->width() - 6, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );

    for ( int i = 0; i < m_sliders.size(); ++i )
    {
        m_sliders[i]->setMin( m_colormap.get( i ).value + 0.01 );
        m_sliders[i]->setMax( m_colormap.get( i + 2 ).value - 0.01 );
    }
    repaint();
}

void ColormapEditWidget::colorChanged( QColor color, int id )
{
    m_colormap.setColor( id, color );

    QImage* image;
    image = createImage( this->width() - 6 );
    QPixmap pix( this->width() - 6, 20 );
    pix.convertFromImage( *image );
    m_cLabel->setPixmap( pix );
}

void ColormapEditWidget::newEntry()
{
    float v1 = m_colormap.get( m_colormap.size() - 2 ).value;
    float v2 = m_colormap.get( m_colormap.size() - 1 ).value;
    float value = v1 + ( ( v2 - v1 ) / 2.0 );
    m_colormap.insertValue( value, QColor( 255, 255, 255 ) );
    redrawWidget();
}

void ColormapEditWidget::removeEntry( int id )
{
    m_colormap.removeValue( id );
    redrawWidget();
}

void ColormapEditWidget::save()
{
    GLFunctions::addColormap( m_colormap );
}
