/*
 * sliderwithedit.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */

#ifndef SLIDERWITHEDITINT_H_
#define SLIDERWITHEDITINT_H_

#include <QtGui/QHBoxLayout>

class QSlider;
class QLineEdit;

class SliderWithEditInt : public QHBoxLayout
{
    Q_OBJECT

public:
    SliderWithEditInt( QString name, QWidget* parent = 0 );
    virtual ~SliderWithEditInt();

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
