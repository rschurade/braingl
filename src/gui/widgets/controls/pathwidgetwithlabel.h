/*
 * pathwidgetwithlabel.h
 *
 * Created on: Mar 6, 2013
 * @author schurade
 */

#ifndef PATHWIDGETWITHLABEL_H_
#define PATHWIDGETWITHLABEL_H_

#include <QDir>
#include <QFrame>

class QLabel;
class QLineEdit;
class QPushButton;

class PathWidgetWithLabel : public QFrame
{
    Q_OBJECT

public:
    PathWidgetWithLabel( QString label, int id = 0, QWidget* parent = 0  );
    virtual ~PathWidgetWithLabel();

    void setValue( QDir value);

private:
    int m_id;
    QDir m_value;

    QLabel* m_label;
    QLineEdit* m_edit;
    QPushButton* m_button;

private slots:
    void slotButton();

signals:
    void valueChanged( QDir, int );
};
#endif /* PATHWIDGETWITHLABEL_H_ */
