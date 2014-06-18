/*
 * fnaction.h
 *
 * Created on: Jan 4, 2013
 * @author Ralph Schurade
 */

#ifndef FNACTION_H_
#define FNACTION_H_

#include "../../../data/enums.h"

#include <QAction>

class FNAction : public QAction
{
    Q_OBJECT

public:
    FNAction( const QIcon &icon, const QString &text, QObject *parent, Fn::Algo algo );
    virtual ~FNAction();

private:
    Fn::Algo m_algo;

private slots:
    void slotTriggered();

signals:
    void sigTriggered( Fn::Algo algo );
};

#endif /* FNACTION_H_ */
