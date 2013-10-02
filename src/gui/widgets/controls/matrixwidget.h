/*
 * matrixwidget.h
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#ifndef MATRIXWIDGET_H_
#define MATRIXWIDGET_H_

#include "../../../data/enums.h"

#include <QFrame>
#include <QMatrix4x4>

class QLineEdit;
class QLabel;

class MatrixWidget  : public QFrame
{
    Q_OBJECT

public:
    MatrixWidget( QString name, int id = -1, QWidget* parent = 0 );
    virtual ~MatrixWidget();

    void setValue( QMatrix4x4 value );

private:
    int m_id;
    int m_digits;

    QLabel* m_label;
    QLineEdit* m_00;
    QLineEdit* m_01;
    QLineEdit* m_02;
    QLineEdit* m_03;
    QLineEdit* m_10;
    QLineEdit* m_11;
    QLineEdit* m_12;
    QLineEdit* m_13;
    QLineEdit* m_20;
    QLineEdit* m_21;
    QLineEdit* m_22;
    QLineEdit* m_23;
    QLineEdit* m_30;
    QLineEdit* m_31;
    QLineEdit* m_32;
    QLineEdit* m_33;

    QLineEdit* makeEdit();

private slots:
    void editChanged();

signals:
    void valueChanged( int, QMatrix4x4 );
};

#endif /* MATRIXWIDGET_H_ */
