/*
 * propertywidget.h
 *
 *  Created on: Aug 27, 2013
 *      Author: schurade
 */

#ifndef PROPERTYWIDGET_H_
#define PROPERTYWIDGET_H_

#include <QFrame>

class QLabel;
class QWidget;

class PropertyWidget : public QFrame
{
public:
    enum Position {
        left,
        top,
        right,
        bottom
    };

    PropertyWidget( QString label = "", int id = - 1, QWidget* parent = 0 );
    virtual ~PropertyWidget();

    void showLabel( bool show );
    void setLabelPosition( Position position );

protected:
    void setup();

    QLabel* m_label;
    int m_id;
    QWidget* m_controlWidget;

private:
    bool m_showLabel;
    Position m_labelPosition;

};

#endif /* PROPERTYWIDGET_H_ */
