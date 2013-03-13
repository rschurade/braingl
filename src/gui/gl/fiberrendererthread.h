/*
 * fiberrendererthread.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph Schurade
 */

#ifndef FIBERRENDERERTHREAD_H_
#define FIBERRENDERERTHREAD_H_

#include <QDebug>
#include <QThread>
#include <QVector>

class FiberRendererThread : public QThread
{
public:
    FiberRendererThread( QVector< QVector< float > >& data, QVector< QVector< float > >& extraData, int id );
    virtual ~FiberRendererThread();

    QVector<float>* getVerts();

private:
    void run();

    QVector< QVector< float > > m_data;
    QVector< QVector< float > > m_extraData;
    int m_id;

    QVector<float>* m_verts;

};

#endif /* FIBERRENDERERTHREAD_H_ */
