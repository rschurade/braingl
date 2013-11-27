/*
 * attractthread.h
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#ifndef ATTRACTTHREAD_H_
#define ATTRACTTHREAD_H_

#include "connections.h"

#include <qthread.h>

class Connections;

class AttractThread: public QThread
{
    Q_OBJECT

public:
    AttractThread(Connections* cons, int id);
    virtual ~AttractThread();
    void run();

private:
    Connections* m_cons;
};

#endif /* ATTRACTTHREAD_H_ */
