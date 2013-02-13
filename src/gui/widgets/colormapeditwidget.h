/*
 * colormapwidget.h
 *
 *  Created on: 12.02.2013
 *      Author: Ralph Schurade
 */

#ifndef COLORMAPEDITWIDGET_H_
#define COLORMAPEDITWIDGET_H_

#include "../../algos/colormapbase.h"

#include <QtGui/QWidget>

class SliderWithEdit;
class ROIPropertyView;
class QVBoxLayout;
class QImage;
class QLabel;
class QPushButton;

class ColormapEditWidget : public QWidget
{
    Q_OBJECT

public:
    ColormapEditWidget( QWidget* parent = 0 );
    virtual ~ColormapEditWidget();

    void resizeEvent( QResizeEvent* event );

private:
    QImage* createImage( int width );
    void redrawWidget();

    ColormapBase m_colormap;

    QLabel* m_cLabel;

    QVector< SliderWithEdit*>m_sliders;


private slots:
    void sliderChanged( float value, int id );
    void colorChanged( QColor color, int id );
    void newEntry( int id );
    void removeEntry( int id );
    void save();
};

#endif /* COLORMAPEDITWIDGET_H_ */
