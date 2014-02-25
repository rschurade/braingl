/*
 * correlation.h
 *
 *  Created on: Sep 11, 2013
 *      Author: schurade
 */

#ifndef CORRELATION_H_
#define CORRELATION_H_

#include <QDebug>
#include <QVector>

class DatasetMeshTimeSeries;
class CorrelationThread;

class Correlation : public QObject
{
    Q_OBJECT

public:
    Correlation( DatasetMeshTimeSeries* ds );
    virtual ~Correlation();

    void start();

    float** getResult();

private:
    DatasetMeshTimeSeries* m_dataset;

    std::vector<CorrelationThread*> m_threads;
    int m_threadsRunning;

    float* m_exField;
    float* m_ex2Field;

    float** m_result;

private slots:
    void slotProgress();
    void slotThreadFinished();

signals:
    void progress();
    void finished();
};



#endif /* CORRELATION_H_ */
