/*
 * selectwithlabel.h
 *
 *  Created on: 12.07.2012
 *      Author: Ralph Schurade
 */

#ifndef SELECTWITHLABEL_H_
#define SELECTWITHLABEL_H_

#include <QVariant>
#include <QFrame>

class QComboBox;
class QLabel;

class SelectWithLabel  : public QFrame
{
    Q_OBJECT

public:
    SelectWithLabel( QString label, int id, QWidget* parent = 0 );
    virtual ~SelectWithLabel();

    void insertItem( int index, QString label );
    void setCurrentIndex( int index );
    int getCurrentIndex();
    void addItem (const QString  &text   , const QVariant  &userData=QVariant() );

private:
    QLabel* m_label;
    QComboBox* m_comboBox;

    int m_id;
    int m_currentIndex;

signals:
    void currentIndexChanged( int, int );

private slots:
    void indexChanged( int index );

};

#endif /* SELECTWITHLABEL_H_ */
