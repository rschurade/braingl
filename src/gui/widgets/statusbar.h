/*
 * statusbar.h
 *
 *  Created on: Nov 13, 2012
 *      Author: Ralph Schurade
 */

#ifndef STATUSBAR_H_
#define STATUSBAR_H_

#include <QtGui>

class StatusBarView;

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    StatusBar( QWidget* parent = 0 );
    virtual ~StatusBar();

    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    StatusBarView* m_statusBarView;

private slots:
    void slotStatusChanged( QString text );
};

#endif /* STATUSBAR_H_ */
