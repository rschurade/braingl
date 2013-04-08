/*
 * sliderwithedit.h
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */

#ifndef SLIDERWITHEDIT_H_
#define SLIDERWITHEDIT_H_

#include <QFrame>

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

   void setDigits( int digits );

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
   int m_digits;
};

#endif /* SLIDERWITHEDIT_H_ */
