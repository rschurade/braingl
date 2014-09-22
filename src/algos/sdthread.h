/*
 * sdthread.h
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#ifndef SDTHREAD_H_
#define SDTHREAD_H_

#include "../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>

class DatasetDWI;

class SDThread : public QThread
{
    Q_OBJECT

public:
    SDThread( DatasetDWI* ds, int id );
    virtual ~SDThread();

private:
    void run();
    ColumnVector calcSD( int id );

    DatasetDWI* m_dataset;

    int m_id;
    int m_blockSize;

    std::vector<float>* m_dwi;
    std::vector<float> m_bvals;
    std::vector<QVector3D> m_bvecs;



signals:
    void progress();
    void finished();
};

#endif /* SDTHREAD_H_ */
