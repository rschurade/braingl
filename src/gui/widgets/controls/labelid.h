/*
 * labelid.h
 *
 *  Created on: Aug 16, 2013
 *      Author: schurade
 */

#ifndef LABELID_H_
#define LABELID_H_

#include <QLabel>

class LabelID : public QLabel
{
    Q_OBJECT

public:
    LabelID( int id, QString text, QWidget* parent = 0 );
    virtual ~LabelID();

private:
    int m_id;

public slots:
    void slotSetHidden( bool state, int id );
};

#endif /* LABELID_H_ */
