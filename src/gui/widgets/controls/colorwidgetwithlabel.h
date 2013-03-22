/*
 * colorwidgetwithlabel.h
 *
 * Created on: Feb 4, 2013
 * @author Ralph Schurade
 */

#ifndef COLORWIDGETWITHLABEL_H_
#define COLORWIDGETWITHLABEL_H_

#include <QColor>
#include <QFrame>

class QLabel;
class QPushButton;

class ColorWidgetWithLabel : public QFrame
{
    Q_OBJECT

public:
    ColorWidgetWithLabel( QString label, int id = 0, QWidget* parent = 0 );
    virtual ~ColorWidgetWithLabel();

    void setValue( QColor value);

private:
    int m_id;
    QColor m_value;

    QLabel* m_label;
    QPushButton* m_button;

private slots:
    void slotButton();

signals:
    void colorChanged( QColor, int );
};

#endif /* COLORWIDGETWITHLABEL_H_ */
