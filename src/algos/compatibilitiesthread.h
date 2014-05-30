/*
 * compatibilitiesthread.h
 *
 *  Created on: Dec 11, 2013
 *      Author: boettgerj
 */

#ifndef COMPATIBILITIESTHREAD_H_
#define COMPATIBILITIESTHREAD_H_

#include "edge.h"
#include "compatibilities.h"

#include <QDebug>
#include <QThread>

#include <QVector3D>

class CompatibilitiesThread: public QThread
{
Q_OBJECT

public:
    CompatibilitiesThread( int id, float c_thr, QList<Edge*> edges, Compatibilities* compatibilities );
    virtual ~CompatibilitiesThread();

private:
    void run();
    int m_id;
    float m_c_thr;
    double vis_c( Edge* ep, Edge* eq );
    QVector3D proj( QVector3D a, QVector3D b, QVector3D p );
    QList<Edge*> m_edges;
    Compatibilities* m_compatibilities;

signals:
    void progress();
    void finished();
};

#endif /* COMPATIBILITIESTHREAD_H_ */
