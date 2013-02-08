/*
 * sliderwithedit.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph Schurade
 */

#ifndef SLIDERWITHEDITINT2_H_
#define SLIDERWITHEDITINT2_H_

#include <QtGui/QFrame>

class QSlider;
class QLineEdit;
class QPushButton;

class SliderWithEditInt2 : public QFrame
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
    void minusPressed();
    void plusPressed();

signals:
    void valueChanged( int value );

private:
   QSlider*  m_slider;
   QLineEdit* m_edit;
   QPushButton* m_button1;
   QPushButton* m_button2;
};

#endif /* SLIDERWITHEDITINT_H_ */
