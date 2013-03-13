/*
 * pushbuttonwithid.h
 *
 *  Created on: Feb 13, 2013
 *      Author: schurade
 */
#include <QPushButton>

#ifndef PUSHBUTTONWITHID_H_
#define PUSHBUTTONWITHID_H_

class PushButtonWithId : public QPushButton
{
    Q_OBJECT

public:
    PushButtonWithId( const QString& text, int id = 0, QWidget* parent = 0 );
    virtual ~PushButtonWithId();

private:
    int m_id;

private slots:
    void slotClicked();
signals:
    void signalClicked( int );
};

#endif /* PUSHBUTTONWITHID_H_ */
