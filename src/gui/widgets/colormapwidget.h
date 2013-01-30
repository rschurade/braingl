/*
 * colormapwidget.h
 *
 *  Created on: Jan 29, 2013
 *      Author: schurade
 */

#ifndef COLORMAPWIDGET_H_
#define COLORMAPWIDGET_H_

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

    QImage* m_image;
    QLabel* m_nlabel;
    QLabel* m_clabel;

    int m_colormap;
    int m_width;
    float m_lowerThreshold;
    float m_upperThreshold;

public slots:
    void setLowerThreshold( float value );
    void setUpperThreshold( float value );
    void setColormap( int value );
};

#endif /* COLORMAPWIDGET_H_ */
