/*
 * selectwithlabel.h
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */

#ifndef SELECTWITHLABEL_H_
#define SELECTWITHLABEL_H_

#include <QtGui/QWidget>

class QComboBox;
class QLabel;

class SelectWithLabel  : public QWidget
{
    Q_OBJECT

public:
    SelectWithLabel( QString label, int id );
    virtual ~SelectWithLabel();

    void insertItem( int index, QString label );
    void setCurrentIndex( int index );

private:
    QLabel* m_label;
    QComboBox* m_comboBox;

    int m_id;

signals:
    void currentIndexChanged( int, int );

private slots:
    void indexChanged( int index );

};

#endif /* SELECTWITHLABEL_H_ */
