/*
 * fiberrendererthread.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph
 */

#ifndef FIBERRENDERERTHREAD_H_
#define FIBERRENDERERTHREAD_H_

#include <QtCore/QDebug>
#include <QThread>
#include <QtCore/QVector>

class FiberRendererThread : public QThread
{
public:
    FiberRendererThread( QVector< QVector< float > >& data, int id );
    virtual ~FiberRendererThread();

    QVector<float>* getVerts();

private:
    void run();

    QVector< QVector< float > > m_data;
    int m_id;

    QVector<float>* m_verts;

};

#endif /* FIBERRENDERERTHREAD_H_ */
