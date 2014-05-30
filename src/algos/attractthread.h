/*
 * attractthread.h
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#ifndef ATTRACTTHREAD_H_
#define ATTRACTTHREAD_H_

#include "edge.h"

#include "compatibilities.h"

#include <qthread.h>

class AttractThread: public QThread
{
    Q_OBJECT

public:
    AttractThread(int id, float bell, QList<Edge*> edges, Compatibilities* compatibilities);
    virtual ~AttractThread();
    void run();

private:
    int m_id;
    float m_bell;
    QList<Edge*> m_edges;
    Compatibilities* m_compatibilities;

signals:
    void progress();
    void finished();
};

#endif /* ATTRACTTHREAD_H_ */
