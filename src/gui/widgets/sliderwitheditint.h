/*
 * sliderwithedit.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */

#ifndef SLIDERWITHEDITINT_H_
#define SLIDERWITHEDITINT_H_

#include <QtGui/QWidget>

class QSlider;
class QLineEdit;
class QLabel;
class QPushButton;

class SliderWithEditInt : public QWidget
{
    Q_OBJECT

public:
    SliderWithEditInt( QString name, int id = 0, QWidget* parent = 0 );
    virtual ~SliderWithEditInt();

    void setValue( int value );
    int getValue();

   void setMin( int min );
   void setMax( int max );
   void setStep( int step );

public slots:
    void sliderMoved( int value );
    void editEdited();
    void minusPressed();
    void plusPressed();

signals:
    void valueChanged( int value, int );

private:
   QSlider*  m_slider;
   QLineEdit* m_edit;
   QPushButton* m_button1;
   QPushButton* m_button2;

   int m_id;

};

#endif /* SLIDERWITHEDITINT_H_ */
