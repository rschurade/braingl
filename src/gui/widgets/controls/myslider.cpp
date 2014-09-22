/*
 * myslider.cpp
 *
 *  Created on: Aug 21, 2014
 *      Author: schurade
 */

#include "myslider.h"

#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>

MySlider::MySlider( QWidget* parent ) :
    QSlider( parent )
{
}

MySlider::~MySlider()
{
}

void MySlider::mousePressEvent ( QMouseEvent * event )
{
  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  if (event->button() == Qt::LeftButton &&
      sr.contains(event->pos()) == false)
  {
    int newVal;
    if (orientation() == Qt::Vertical)
       newVal = minimum() + ((maximum()-minimum()) * (height()-event->y())) / height();
    else
       newVal = minimum() + ((maximum()-minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
        setValue( maximum() - newVal );
    else
        setValue(newVal);

    emit( sliderMoved( newVal ) );
    event->accept();
  }
  QSlider::mousePressEvent(event);
}
