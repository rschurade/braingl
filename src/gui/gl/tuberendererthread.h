/*
 * tuberendererthread.h
 *
 *  Created on: 26.02.2013
 *      Author: Ralph Schurade
 */

#ifndef TUBERENDERERTHREAD_H_
#define TUBERENDERERTHREAD_H_

#include <QDebug>
#include <QThread>
#include <QVector>

class TubeRendererThread : public QThread
{
public:
    TubeRendererThread( QVector< QVector< float > >& data, QVector< QVector< float > >& extraData, int id );
    virtual ~TubeRendererThread();

    QVector<float>* getVerts();

private:
    void run();

    QVector< QVector< float > > m_data;
    QVector< QVector< float > > m_extraData;
    int m_id;

    QVector<float>* m_verts;

};

#endif /* TUBERENDERERTHREAD_H_ */
