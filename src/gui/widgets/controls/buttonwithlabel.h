/*
 * buttonwidgetwithlabel.h
 *
 * Created on: Aug 20, 2013
 * @author schurade
 */

#ifndef BUTTONWITHLABEL_H_
#define BUTTONWITHLABEL_H_

#include <QFrame>

class QLabel;
class QPushButton;

class ButtonWithLabel : public QFrame
{
    Q_OBJECT

public:
    ButtonWithLabel( QString label, int id = 0, QWidget* parent = 0  );
    virtual ~ButtonWithLabel();

private:
    int m_id;

    QLabel* m_label;
    QPushButton* m_button;

private slots:
    void slotButton();

signals:
    void buttonPressed( int );
};
#endif /* BUTTONWITHLABEL_H_ */
