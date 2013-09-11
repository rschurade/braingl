/*
 * correlationthread.h
 *
 *  Created on: Sep 11, 2013
 *      Author: schurade
 */

#ifndef CORRELATIONTHREAD_H_
#define CORRELATIONTHREAD_H_

#include <QDebug>
#include <QThread>

class DatasetMeshTimeSeries;

class CorrelationThread : public QThread
{
    Q_OBJECT

public:
    CorrelationThread( int id, DatasetMeshTimeSeries* ds );
    virtual ~CorrelationThread();

    QVector< QVector<float> >* getResult();

private:
    void run();

    int m_id;
    DatasetMeshTimeSeries* m_dataset;

    QVector< QVector<float> >m_result;

signals:
    void progress();
    void finished();
};

#endif /* CORRELATIONTHREAD_H_ */
