/*
 * bundlingthread.h
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#ifndef BUNDLINGTHREAD_H_
#define BUNDLINGTHREAD_H_

#include "connections.h"

#include <QDebug>
#include <QThread>

class Connections;

class BundlingThread : public QThread
{
    Q_OBJECT

public:
    BundlingThread(Connections* cons);
    virtual ~BundlingThread();
    void run();

private:
    Connections* m_cons;
};

#endif /* BUNDLINGTHREAD_H_ */
