/*
 * sliderwithedit.h
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */

#ifndef SLIDERWITHEDITINT_H_
#define SLIDERWITHEDITINT_H_

#include <QFrame>

class MySlider;
class QLineEdit;
class QLabel;
class QPushButton;

class SliderWithEditInt : public QFrame
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
    void increment();
    void decrement();
    void minEdited();
    void maxEdited();

signals:
    void valueChanged( int value, int );
    void minChanged( QVariant value );
    void maxChanged( QVariant value );

private:
    QLabel* m_label;
    MySlider*  m_slider;
    QLineEdit* m_edit;
    QLineEdit* m_edit1;
    QLineEdit* m_edit2;
    QPushButton* m_button1;
    QPushButton* m_button2;

    int m_id;

};

#endif /* SLIDERWITHEDITINT_H_ */
