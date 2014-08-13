/*
 * vectorwidget.h
 *
 *  Created on: Jul 18, 2014
 *      Author: schurade
 */

#ifndef VECTORWIDGET_H_
#define VECTORWIDGET_H_

#include "../../../data/enums.h"

#include <QFrame>
#include <QVector3D>

class QLineEdit;
class QLabel;

class VectorWidget  : public QFrame
{
    Q_OBJECT

public:
    VectorWidget( QString name, int id = -1, QWidget* parent = 0 );
    virtual ~VectorWidget();

    void setValue( QVector3D value );

private:
    int m_id;
    int m_digits;

    QLabel* m_label;
    QLineEdit* m_00;
    QLineEdit* m_01;
    QLineEdit* m_02;

    QLineEdit* makeEdit();

private slots:
    void editChanged();

signals:
    void valueChanged( int, QVector3D );
};

#endif /* VECTORWIDGET_H_ */
