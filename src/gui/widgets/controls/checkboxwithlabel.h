/*
 * checkboxwithlabel.h
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#ifndef CHECKBOXWITHLABEL_H_
#define CHECKBOXWITHLABEL_H_

#include <QtGui/QFrame>

class QCheckBox;
class QLabel;


class CheckboxWithLabel  : public QFrame
{
    Q_OBJECT

public:
    CheckboxWithLabel(  QString label, int id = 0, QWidget* parent = 0 );
    virtual ~CheckboxWithLabel();

    void setChecked( bool state );

private:
    QLabel* m_label;
    QCheckBox* m_checkBox;

    int m_id;

signals:
    void stateChanged( int, int );

private slots:
    void slotStateChanged( int );
};

#endif /* CHECKBOXWITHLABEL_H_ */