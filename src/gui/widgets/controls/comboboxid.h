/*
 * comboboxid.h
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#ifndef COMBOBOXID_H_
#define COMBOBOXID_H_

#include <QComboBox>

class ComboBoxID : public QComboBox
{
    Q_OBJECT

public:
    ComboBoxID( int id, QWidget* parent );
    virtual ~ComboBoxID();

private:
    int m_id;

signals:
    void currentIndexChanged( int, int );

private slots:
    void slotIndexChanged( int index );
};

#endif /* COMBOBOXID_H_ */
