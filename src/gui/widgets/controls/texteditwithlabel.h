/*
 * texteditwithlabel.h
 *
 * Created on: 15.11.2013
 * @author Ralph Schurade
 */

#ifndef TEXTEDITWITHLABEL_H_
#define TEXTEDITWITHLABEL_H_

#include <QFrame>

class QTextEdit;
class QLabel;


class TextEditWithLabel  : public QFrame
{
    Q_OBJECT

public:
    TextEditWithLabel(  QString label, int id = 0, QWidget* parent = 0 );
    virtual ~TextEditWithLabel();

    QString text();
    void setText( QString text );

private:
    QLabel* m_label;
    QTextEdit* m_textEdit;

    int m_id;

signals:
    void valueChanged( QString value, int id );

private slots:
    void slotTextChanged();
};

#endif /* TEXTEDITWITHLABEL_H_ */
