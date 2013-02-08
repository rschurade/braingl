/*
 * sliderwithedit.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph Schurade
 */

#ifndef SLIDERWITHEDIT_H_
#define SLIDERWITHEDIT_H_

#include <QtGui/QFrame>

class QSlider;
class QLineEdit;
class QLabel;

class SliderWithEdit : public QFrame
{
    Q_OBJECT

public:
    SliderWithEdit( QString name, int id = 0, QWidget* parent = 0 );
    virtual ~SliderWithEdit();

    void setValue( float value );
    float getValue();

   void setMin( float min );
   void setMax( float max );

public slots:
    void sliderChanged( int value );
    void sliderMoved( int value );
    void editEdited();

signals:
    void valueChanged( float value, int id );

private:
   QSlider*  m_slider;
   QLineEdit* m_edit;
   QLabel* m_label;

   int m_id;
};

#endif /* SLIDERWITHEDIT_H_ */
