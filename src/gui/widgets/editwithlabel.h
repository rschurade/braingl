/*
 * checkboxwithlabel.h
 *
 *  Created on: 14.07.2012
 *      Author: Ralph
 */

#ifndef EDITWITHLABEL_H_
#define EDITWITHLABEL_H_

#include <QtGui/QWidget>

class QLineEdit;
class QLabel;


class EditWithLabel  : public QWidget
{
    Q_OBJECT

public:
    EditWithLabel(  QString label, int id = 0 );
    virtual ~EditWithLabel();

    void setText( QString text );

private:
    QLabel* m_label;
    QLineEdit* m_lineEdit;

    int m_id;

signals:
    void valueChanged( QString value, int id );

private slots:
    void slotTextChanged( QString text );
};

#endif /* CHECKBOXWITHLABEL_H_ */
