/*
 * sd.h
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#ifndef SD_H_
#define SD_H_

#include <QObject>
#include <QVector>

class DatasetDWI;
class SDThread;

class SD : public QObject
{
    Q_OBJECT

public:
    SD( DatasetDWI* ds );
    virtual ~SD();

    void start();

private:
    DatasetDWI* m_dataset;

    QVector<SDThread*> m_threads;
    int m_threadsRunning;



private slots:
    void slotProgress();
    void slotThreadFinished();

signals:
    void progress();
    void finished();
};

#endif /* SD_H_ */
