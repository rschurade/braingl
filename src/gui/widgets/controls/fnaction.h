/*
 * fnaction.h
 *
 *  Created on: Jan 4, 2013
 *      Author: schurade
 */

#ifndef FNACTION_H_
#define FNACTION_H_

#include <QtGui/QtGui>

#include "../../../data/enums.h"

class FNAction : public QAction
{
    Q_OBJECT

public:
    FNAction( const QIcon &icon, const QString &text, QObject *parent, FN_ALGO algo );
    virtual ~FNAction();

private:
    FN_ALGO m_algo;

private slots:
    void slotTriggered();

signals:
    void sigTriggered( FN_ALGO algo );
};

#endif /* FNACTION_H_ */
