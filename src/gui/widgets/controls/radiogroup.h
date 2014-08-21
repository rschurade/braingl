/*
 * radiogroup.h
 *
 *  Created on: 21.08.2014
 *      Author: Ralph
 */

#ifndef RADIOGROUP_H_
#define RADIOGROUP_H_

#include <QFrame>
#include <QList>
#include <QString>
#include <QVariant>

class QRadioButton;
class QLabel;


class RadioGroup : public QFrame
{
    Q_OBJECT

public:
    RadioGroup( QString label, QList<QString>options, int id = 0, QWidget* parent = 0 );
    virtual ~RadioGroup();

    void addItem (const QString &text );
    void setCurrentIndex( int index );
    int getCurrentIndex();
    QString getSelectedItemString();

private:
    QLabel* m_label;

    int m_id;

    QList<QRadioButton*>m_buttons;

signals:
    void currentIndexChanged( int, int );

private slots:
    void indexChanged( bool checked );

};

#endif /* RADIOGROUP_H_ */
