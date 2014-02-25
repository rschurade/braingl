/*
 * tuberendererthread.h
 *
 * Created on: 26.02.2013
 * @author Ralph Schurade
 */

#ifndef TUBERENDERERTHREAD_H_
#define TUBERENDERERTHREAD_H_

#include "../../algos/fib.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QVector3D>

class TubeRendererThread : public QThread
{
public:
    TubeRendererThread( std::vector<Fib>* fibs, int id );
    virtual ~TubeRendererThread();

    std::vector<float>* getVerts();

private:
    void run();

    std::vector<Fib>* m_fibs;
    int m_id;

    std::vector<float>* m_verts;
};

#endif /* TUBERENDERERTHREAD_H_ */
