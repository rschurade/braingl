/*
 * colormapwidget.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: Ralph Schurade
 */
#include "colormapwidget.h"

#include <QtGui/QtGui>

ColormapWidget::ColormapWidget( int width ) :
    m_colormap( Fn::ColormapEnum::GRAY ),
    m_width( width ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 ),
    m_min( 0.0 ),
    m_max( 0.0 )
{
    QVBoxLayout* vLayout = new QVBoxLayout();

//    QHBoxLayout* hLayout = new QHBoxLayout();
//    m_nlabel = new QLabel( QString("colormap"), this );
//    hLayout->addWidget( m_nlabel );
//    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    m_nlabel = new QLabel( this );

    m_image = createImage( width );
    QPixmap pix( width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
    hLayout2->addWidget( m_nlabel );

    //vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout2 );
    vLayout->setContentsMargins( 1,1,1,1 );

    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }"
                   );
}

ColormapWidget::~ColormapWidget()
{
}

QImage* ColormapWidget::createImage( int width )
{
    QImage* image = new QImage( width, 20, QImage::Format_RGB32 );
    for ( int i = 0; i < width; ++i )
    {
        QColor c;
        int g = 0;
        switch ( m_colormap )
        {
            case Fn::ColormapEnum::RAINBOW1:
                c = colormap1( (float)i / (float)width );
                break;
            case Fn::ColormapEnum::RAINBOW2:
                c = colormap2( (float)i / (float)width );
                break;
            case Fn::ColormapEnum::BLUEWHITERED:
                c = colormap3( (float)i / (float)width );
                break;
            default:
                g = qMin( 255.0f, qMax( 0.0f, ( (float)i / (float)width - m_lowerThreshold ) / ( m_upperThreshold - m_lowerThreshold ) * 255 ) );
                c = QColor( g, g, g );
                break;
        }
        for ( int k = 0; k < 20; ++k )
        {
            image->setPixel( i, k, c.rgba() );
        }
    }

    return image;
}

QColor ColormapWidget::colormap1( float value )
{
    value = ( value - m_lowerThreshold ) / ( m_upperThreshold - m_lowerThreshold );

    value = qMax( 0.0f, qMin( 1.0f, value ) );
    value *= 5.0;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    if( value < 1.0 )
    {
        g = value;
        b = 1.0f;
    }
    else if( value < 2.0 )
    {
        g = 1.0;
        b = 2.0 -value;
    }
    else if( value < 3.0 )
    {
        r = value-2.0;
        g = 1.0;
    }
    else if( value < 4.0 )
    {
        r = 1.0;
        g = 4.0-value;
    }
    else
    {
        r = 1.0;
        b = value-4.0;
    }
    return QColor( 255 * r, 255 * g, 255 * b );
}

QColor ColormapWidget::colormap2( float value )
{
    value = ( value - m_lowerThreshold ) / ( m_upperThreshold - m_lowerThreshold );

    value = qMax( 0.0f, qMin( 1.0f, value ) );
    value *= 6.0;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    if( value < 2.0 )
    {
        g = value * 0.5f;
        b = 1.0f;
    }
    else if( value < 3.0 )
    {
        g = 1.0;
        b = 3.0 -value;
    }
    else if( value < 4.0 )
    {
        r = value-3.0;
        g = 1.0;
    }
    else if( value < 6.0 )
    {
        r = 1.0;
        g = 0.5f * ( 6.0-value );
    }
    else
    {
        r = 1.0;
    }
    return QColor( 255 * r, 255 * g, 255 * b );
}

QColor ColormapWidget::colormap3( float value )
{
    value = ( value - m_lowerThreshold ) / ( m_upperThreshold - m_lowerThreshold );

    value = qMax( 0.0f, qMin( 1.0f, value ) );
    value *= 2.0;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    if( value < 1.0 )
    {
        r = value;
        g = value;
        b = 1.0;
    }
    else if( value <= 2.0 )
    {
        r = 1.0;
        g = 2.0-value;
        b = 2.0-value;
    }
    else
    {
        r = 1.0;
    }
    return QColor( 255 * r, 255 * g, 255 * b );
}

void ColormapWidget::setLowerThreshold( float value )
{
    m_lowerThreshold = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}

void ColormapWidget::setUpperThreshold( float value )
{
    m_upperThreshold = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}

void ColormapWidget::setMin( float value )
{
    m_min = value;
}

void ColormapWidget::setMax( float value )
{
    m_max = value;
}


void ColormapWidget::setColormap( int value )
{
    m_colormap = qMin( (Fn::ColormapEnum )value, Fn::ColormapEnum::NONE );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}
