/*
 * lineeditid.h
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#ifndef LINEEDITID_H_
#define LINEEDITID_H_

#include <QLineEdit>

class LineEditID : public QLineEdit
{
    Q_OBJECT

public:
    LineEditID( int id, QWidget* parent );
    virtual ~LineEditID();

private:
    int m_id;

public slots:
    void setEnabled2( bool enable, int id );
    void setText2( QString text, int id );
    void slotTextEdited( QString text );

signals:
    void textEdited2( QString text, int id );
};

#endif /* LINEEDITID_H_ */
