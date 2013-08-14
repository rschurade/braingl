/*
 * checkboxid.h
 *
 *  Created on: Aug 14, 2013
 *      Author: schurade
 */

#ifndef CHECKBOXID_H_
#define CHECKBOXID_H_

#include <QCheckBox>

class CheckBoxID : public QCheckBox
{
    Q_OBJECT

public:
    CheckBoxID( int id, QWidget* parent );
    virtual ~CheckBoxID();

private:
    int m_id;

signals:
    void signalStateChanged( int id, int state );

private slots:
    void slotStateChanged( int state );
};

#endif /* CHECKBOXID_H_ */
