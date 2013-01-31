/*
 * colormapwidget.h
 *
 *  Created on: Jan 29, 2013
 *      Author: schurade
 */

#ifndef COLORMAPWIDGET_H_
#define COLORMAPWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui/QFrame>

class QImage;
class QLabel;

class ColormapWidget : public QFrame
{
    Q_OBJECT

public:
    ColormapWidget( int width );
    virtual ~ColormapWidget();

private:
    QImage* createImage( int width );
    QColor colormap1( float value );
    QColor colormap2( float value );
    QColor colormap3( float value );

    QImage* m_image;
    QLabel* m_nlabel;
    QLabel* m_clabel;

    FN_COLORMAP m_colormap;
    int m_width;
    float m_lowerThreshold;
    float m_upperThreshold;
    float m_min;
    float m_max;

public slots:
    void setLowerThreshold( float value );
    void setUpperThreshold( float value );
    void setMin( float value );
    void setMax( float value );
    void setColormap( int value );
};

#endif /* COLORMAPWIDGET_H_ */
