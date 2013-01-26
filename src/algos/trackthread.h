/*
 * trackthread.h
 *
 *  Created on: 25.12.2012
 *      Author: Ralph
 */

#ifndef TRACKTHREAD_H_
#define TRACKTHREAD_H_

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QThread>

#include "../thirdparty/newmat10/newmat.h"

class TrackThread : public QThread
{
    Q_OBJECT

public:
    TrackThread(  QVector<Matrix>* tensors,
                  QVector<Matrix>* logTensors,
                  QVector<float>* fa,
                  QVector<QVector3D>* evec1,
                  int nx,
                  int ny,
                  int nz,
                  float dx,
                  float dy,
                  float dz,
                  int id );

    virtual ~TrackThread();

    QVector< QVector< float > >getFibs();

private:
    void run();

    QVector<float> track( int id, bool negDir );

    int getID( float x, float y, float z );
    void getXYZ( int id, int &x, int &y, int &z );
    float getInterpolatedFA( int id, float inx, float iny, float inz );
    Matrix getInterpolatedTensor( int id, float inx, float iny, float inz );

    QVector<Matrix>* m_tensors;
    QVector<Matrix>* m_logTensors;
    // calculated fa and eigen vectors
    QVector<float>* m_fa;
    QVector<QVector3D>* m_evec1;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    int m_id;

    int m_minLength;
    float m_minFA;
    float m_minStartFA;
    float m_stepSize;
    float m_diag;
    int maxStepsInVoxel;
    float m_smoothness;
    int m_blockSize;

    QVector< QVector< float > >fibs;

signals:
    void progress();
    void finished();
};

#endif /* TRACKTHREAD_H_ */
