/*
 * colormapeditwidget.cpp
 *
 * Created on: 12.02.2013
 * @author Ralph Schurade
 */
#include "colormapeditwidget.h"

#include "../widgets/controls/checkbox.h"
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
#include "../../data/models.h"

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
        QWidget().setLayout( layout() );
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

    QScrollArea* scrollArea = new QScrollArea( this );
    scrollArea->setWidgetResizable( true );
    scrollArea->setWidget( m_buttonPanel );

    vLayout->addWidget( scrollArea );


    QHBoxLayout* hLayout5 = new QHBoxLayout();
    hLayout5->addStretch();
    CheckBox* contUpdating = new CheckBox( tr("continuous updating") );
    contUpdating->setChecked( false );
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
    m_sliders.clear();

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

    int i = 0;
    {
        QHBoxLayout* hLayout2 = new QHBoxLayout();
        PushButtonWithId* insertButton = new PushButtonWithId( "+", i );
        insertButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( insertButton, SIGNAL( signalClicked( int ) ), this, SLOT( newEntry( int ) ) );
        insertButton->setDisabled( true );

        PushButtonWithId* deleteButton = new PushButtonWithId( "-", i );
        deleteButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( deleteButton, SIGNAL( signalClicked( int ) ), this, SLOT( removeEntry( int ) ) );
        deleteButton->setDisabled( true );

        PushButtonWithId* upButton = new PushButtonWithId( "^", i );
        upButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        upButton->setDisabled( true );
        //connect( upButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveUp( int ) ) );

        PushButtonWithId* downButton = new PushButtonWithId( "v", i );
        downButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( downButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveDown( int ) ) );

        SliderWithEdit* slider = new SliderWithEdit( tr(""), Fn::Position::EAST, i );
        m_sliders.push_back( slider );
        slider->setMin( 0.0f );
        slider->setMax( m_colormap.get( i + 1 ).value );
        slider->setValue( m_colormap.get( i ).value );
        connect( slider, SIGNAL( valueChanged( float, int ) ), this, SLOT( sliderChanged( float, int ) ) );


        ColorWidgetWithLabel* colorWidget = new ColorWidgetWithLabel( QString::number( i ), i );
        colorWidget->setValue( m_colormap.get( i ).color );
        connect( colorWidget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );

        hLayout2->addWidget( insertButton );
        hLayout2->addWidget( deleteButton );
        hLayout2->addWidget( upButton );
        hLayout2->addWidget( downButton );
        hLayout2->addWidget( slider );
        hLayout2->addWidget( colorWidget );
        vLayout->addLayout( hLayout2 );
    }


    for ( int i = 1; i < m_colormap.size() - 1; ++i )
    {
        QHBoxLayout* hLayout4 = new QHBoxLayout();
        PushButtonWithId* insertButton = new PushButtonWithId( "+", i );
        hLayout4->addWidget( insertButton );
        insertButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( insertButton, SIGNAL( signalClicked( int ) ), this, SLOT( newEntry( int ) ) );

        PushButtonWithId* deleteButton = new PushButtonWithId( "-", i );
        hLayout4->addWidget( deleteButton );
        deleteButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( deleteButton, SIGNAL( signalClicked( int ) ), this, SLOT( removeEntry( int ) ) );

        PushButtonWithId* upButton = new PushButtonWithId( "^", i );
        hLayout4->addWidget( upButton );
        upButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( upButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveUp( int ) ) );

        PushButtonWithId* downButton = new PushButtonWithId( "v", i );
        hLayout4->addWidget( downButton );
        downButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( downButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveDown( int ) ) );

        SliderWithEdit* slider = new SliderWithEdit( tr(""), Fn::Position::EAST, i );
        m_sliders.push_back( slider );
        slider->setMin( m_colormap.get( i - 1 ).value );
        slider->setMax( m_colormap.get( i + 1 ).value );
        slider->setValue( m_colormap.get( i ).value );
        connect( slider, SIGNAL( valueChanged( float, int ) ), this, SLOT( sliderChanged( float, int ) ) );
        hLayout4->addWidget( slider );

        ColorWidgetWithLabel* colorWidget = new ColorWidgetWithLabel( QString::number( i ), i );
        colorWidget->setValue( m_colormap.get( i ).color );
        connect( colorWidget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );
        hLayout4->addWidget( colorWidget );

        vLayout->addLayout( hLayout4 );
    }

    {
        i = m_colormap.size() - 1;
        QHBoxLayout* hLayout2 = new QHBoxLayout();
        PushButtonWithId* insertButton = new PushButtonWithId( "+", i );
        insertButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( insertButton, SIGNAL( signalClicked( int ) ), this, SLOT( newEntry( int ) ) );

        PushButtonWithId* deleteButton = new PushButtonWithId( "-", i );
        deleteButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( deleteButton, SIGNAL( signalClicked( int ) ), this, SLOT( removeEntry( int ) ) );
        deleteButton->setDisabled( true );

        PushButtonWithId* upButton = new PushButtonWithId( "^", i );
        upButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( upButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveUp( int ) ) );

        PushButtonWithId* downButton = new PushButtonWithId( "v", i );
        downButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        downButton->setDisabled( true );
        //connect( downButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveDown( int ) ) );

        SliderWithEdit* slider = new SliderWithEdit( tr(""), Fn::Position::EAST, i );
        m_sliders.push_back( slider );
        slider->setMin( m_colormap.get( i - 1 ).value );
        slider->setMax( 1.0f );
        slider->setValue( m_colormap.get( i ).value );
        connect( slider, SIGNAL( valueChanged( float, int ) ), this, SLOT( sliderChanged( float, int ) ) );


        ColorWidgetWithLabel* colorWidget = new ColorWidgetWithLabel( QString::number( i ), i );
        colorWidget->setValue( m_colormap.get( i ).color );
        connect( colorWidget, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( colorChanged( QColor, int ) ) );

        hLayout2->addWidget( insertButton );
        hLayout2->addWidget( deleteButton );
        hLayout2->addWidget( upButton );
        hLayout2->addWidget( downButton );
        hLayout2->addWidget( slider );
        hLayout2->addWidget( colorWidget );
        vLayout->addLayout( hLayout2 );
    }

    vLayout->addStretch();
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

    for ( int i = 1; i < m_sliders.size() - 1; ++i )
    {
        m_sliders[i]->setMin( m_colormap.get( i - 1 ).value );
        m_sliders[i]->setMax( m_colormap.get( i + 1 ).value );
    }
    m_sliders.first()->setMax( m_colormap.get( 1 ).value );
    m_sliders.last()->setMin( m_colormap.get( m_colormap.size() - 2 ).value );

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

    Models::g()->submit();
}

void ColormapEditWidget::update()
{
    m_colormap.setName( m_nameEdit->text() );
    ColormapFunctions::updateColormap( m_selected, m_colormap );
    GLFunctions::reloadShaders();

    Models::g()->submit();
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

void ColormapEditWidget::moveUp( int id )
{
    QColor color1 = m_colormap.get( id ).color;
    QColor color2 = m_colormap.get( id - 1 ).color;

    m_colormap.setColor( id, color2 );
    m_colormap.setColor( id - 1, color1 );

    redrawWidget();
}

void ColormapEditWidget::moveDown( int id )
{
    QColor color1 = m_colormap.get( id ).color;
    QColor color2 = m_colormap.get( id + 1 ).color;

    m_colormap.setColor( id, color2 );
    m_colormap.setColor( id + 1, color1 );

    redrawWidget();
}
