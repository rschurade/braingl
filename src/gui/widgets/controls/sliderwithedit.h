/*
 * sliderwithedit.h
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */

#ifndef SLIDERWITHEDIT_H_
#define SLIDERWITHEDIT_H_

#include "../../../data/enums.h"

#include <QFrame>

class MySlider;
class QLineEdit;
class QLabel;
class QHBoxLayout;

class SliderWithEdit : public QFrame
{
    Q_OBJECT

public:
    SliderWithEdit( QString name, Fn::Position editPos = Fn::Position::NORTH_EAST, int id = 0, QWidget* parent = 0 );
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
    void minEdited();
    void maxEdited();

signals:
    void valueChanged( float value, int id );
    void minChanged( QVariant value );
    void maxChanged( QVariant value );

private:
   MySlider*  m_slider;
   QLineEdit* m_edit;
   QLineEdit* m_edit1;
   QLineEdit* m_edit2;
   QLabel* m_label;

   int m_id;
   int m_digits;
};

#endif /* SLIDERWITHEDIT_H_ */
