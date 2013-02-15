/*
 * colormapwidget.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: Ralph Schurade
 */
#include "colormapwidget.h"
#include "../../gl/glfunctions.h"
#include "../../gl/colormapfunctions.h"
#include "../../../algos/colormapbase.h"

#include <QtGui/QtGui>

ColormapWidget::ColormapWidget( int width, float min, float max ) :
    m_colormap( 0 ),
    m_width( width ),
    m_min( min ),
    m_max( max ),
    m_selectedMin( 0.0 ),
    m_selectedMax( 0.0 ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 )
{
    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_clabel = new QLabel( this );

    m_image = createImage( width );
    QPixmap pix( width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
    hLayout->addStretch();
    hLayout->addWidget( m_clabel );
    hLayout->addStretch();

    vLayout->addLayout( hLayout );
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

void ColormapWidget::resizeEvent( QResizeEvent* event )
{
    m_width = this->width() - 6;
    QImage* image;
    image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *image );
    m_clabel->setPixmap( pix );
}

QImage* ColormapWidget::createImage( int width )
{
    QImage* image = new QImage( width, 20, QImage::Format_RGB32 );
    for ( int i = 0; i < width; ++i )
    {
        QColor c;
        ColormapBase cmap = ColormapFunctions::getColormap( m_colormap );

        float value = ( ( (float)i / (float)width ) - m_selectedMin ) / ( m_selectedMax - m_selectedMin );
        c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, value ) ) );

        float compare = ( (float)i / (float)width );
        if ( ( compare > m_selectedMin - 0.005 && compare < m_selectedMin ) ||
             ( compare > m_selectedMax && compare < m_selectedMax + 0.005 ) )
        {
            c = getInverseColor( c );
        }

        if ( compare < m_lowerThreshold || compare > m_upperThreshold )
        {
            c = Qt::gray;
        }

        for ( int k = 0; k < 20; ++k )
        {
            image->setPixel( i, k, c.rgba() );
        }
    }

    return image;
}

QColor ColormapWidget::getInverseColor( QColor color )
{
    int red = 255 - color.red();
    int green = 255 - color.green();
    int blue = 255 - color.blue();
    return QColor( red, green, blue );
}

void ColormapWidget::setLowerThreshold( float value )
{
    m_lowerThreshold = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
}

void ColormapWidget::setUpperThreshold( float value )
{
    m_upperThreshold = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
}

void ColormapWidget::setMin( float value )
{
    m_selectedMin = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
}

void ColormapWidget::setMax( float value )
{
    m_selectedMax = qMax( 0.0f, qMin( 1.0f, value / m_max ) );
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
}


void ColormapWidget::setColormap( int value )
{
    m_colormap = value;
    m_image = createImage( m_width );
    QPixmap pix( m_width, 20 );
    pix.convertFromImage( *m_image );
    m_clabel->setPixmap( pix );
}
