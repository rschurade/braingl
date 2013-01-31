/*
 * colormapwidget.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: schurade
 */
#include <QtGui/QtGui>

#include "colormapwidget.h"

ColormapWidget::ColormapWidget( int width ) :
    m_colormap( 0 ),
    m_width( width ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 )
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
        if ( m_colormap == 1 )
        {
            c = colormap1( (float)i / (float)width );
        }
        else if ( m_colormap == 2 )
        {
            c = colormap2( (float)i / (float)width );
        }
        else
        {
            int g = (float)i / (float)width * 255;
            c = QColor( g, g, g );
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
    value = qMax( 0.0f, qMin( 1.0f, value ) );
    m_lowerThreshold = value;
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}

void ColormapWidget::setUpperThreshold( float value )
{
    value = qMax( 0.0f, qMin( 1.0f, value ) );
    m_upperThreshold = value;
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}

void ColormapWidget::setColormap( int value )
{
    value = qMax( 0, qMin( value, 3 ) );
    m_colormap = value;
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_nlabel->setPixmap( pix );
}
