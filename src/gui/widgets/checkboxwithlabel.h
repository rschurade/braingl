/*
 * checkboxwithlabel.h
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#ifndef CHECKBOXWITHLABEL_H_
#define CHECKBOXWITHLABEL_H_

#include <QtGui/QWidget>

class QCheckBox;
class QLabel;


class CheckboxWithLabel  : public QWidget
{
    Q_OBJECT

public:
    CheckboxWithLabel(  QString label );
    virtual ~CheckboxWithLabel();

    void setChecked( bool state );

private:
    QLabel* m_label;
    QCheckBox* m_checkBox;

signals:
        void stateChanged( int );

private slots:
    void slotStateChanged( int );
};

#endif /* CHECKBOXWITHLABEL_H_ */
