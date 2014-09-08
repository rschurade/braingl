/*
 * myslider.h
 *
 *  Created on: Aug 21, 2014
 *      Author: schurade
 */

#ifndef MYSLIDER_H_
#define MYSLIDER_H_

#include <QSlider>

class MySlider : public QSlider
{
public:
    MySlider( QWidget* parent = 0 );
    virtual ~MySlider();

    void mousePressEvent ( QMouseEvent * event );
};

#endif /* MYSLIDER_H_ */
