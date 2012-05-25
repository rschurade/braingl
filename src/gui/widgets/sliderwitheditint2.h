/*
 * sliderwithedit.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */

#ifndef SLIDERWITHEDITINT2_H_
#define SLIDERWITHEDITINT2_H_

#include <QtGui/QWidget>

class QSlider;
class QLineEdit;

class SliderWithEditInt2 : public QWidget
{
    Q_OBJECT

public:
    SliderWithEditInt2( QWidget* parent = 0 );
    virtual ~SliderWithEditInt2();

    void setValue( int value );
    int getValue();

   void setMin( int min );
   void setMax( int max );

public slots:
    void sliderMoved( int value );
    void editEdited();

signals:
    void valueChanged( int value );

private:
   QSlider*  m_slider;
   QLineEdit* m_edit;
};

#endif /* SLIDERWITHEDITINT_H_ */
