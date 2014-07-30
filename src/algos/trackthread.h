/*
 * trackthread.h
 *
 * Created on: 25.12.2012
 * @author Ralph Schurade
 */

#ifndef TRACKTHREAD_H_
#define TRACKTHREAD_H_

#include "fib.h"

#include "../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QVector3D>

class TrackThread : public QThread
{
    Q_OBJECT

public:
    TrackThread(  std::vector<Matrix>* tensors,
                  std::vector<Matrix>* logTensors,
                  std::vector<float>* fa,
                  std::vector<QVector3D>* evec1,
                  int nx,
                  int ny,
                  int nz,
                  float dx,
                  float dy,
                  float dz,
                  int id,
                  int minLength,
                  float minFA,
                  float minStartFA,
                  float stepSize,
                  float smoothness );

    virtual ~TrackThread();

    std::vector<Fib>*getFibs();

private:
    void run();

    void track( int id, bool negDir, Fib& result );

    int getID( float x, float y, float z );
    void getXYZ( int id, int &x, int &y, int &z );
    float getInterpolatedFA( int id, float inx, float iny, float inz );
    Matrix getInterpolatedTensor( int id, float inx, float iny, float inz );

    std::vector<Matrix>* m_tensors;
    std::vector<Matrix>* m_logTensors;
    // calculated fa and eigen vectors
    std::vector<float>* m_fa;
    std::vector<QVector3D>* m_evec1;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    int m_id;

    unsigned int m_minLength;
    float m_minFA;
    float m_minStartFA;
    float m_stepSize;
    float m_diag;
    int maxStepsInVoxel;
    float m_smoothness;
    int m_blockSize;

    std::vector<Fib>m_fibs;

signals:
    void progress();
    void finished();
};

#endif /* TRACKTHREAD_H_ */
