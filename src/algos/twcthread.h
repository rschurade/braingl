/*
 * twcthread.h
 *
 * Created on: Jan 28, 2013
 * @author Ralph Schurade
 */

#ifndef TWCTHREAD_H_
#define TWCTHREAD_H_

#include "fib.h"

#include "../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QVector3D>

class DatasetTensor;

class TWCThread : public QThread
{
    Q_OBJECT

public:
    TWCThread( int id,
                std::vector<float>* mask,
                std::vector<Matrix>* logtensors1,
                std::vector<Matrix>* logtensors2,
                std::vector<Matrix>* logtensors3,
                std::vector<QVector3D>* evec1,
                std::vector<QVector3D>* evec2,
                std::vector<QVector3D>* evec3,
                int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~TWCThread();

    std::vector<Fib>* getFibs();

private:
    void run();

    void track( int id, bool negDir, Fib& result );

    float getInterpolatedFA( int &id, float &inx, float &iny, float &inz );
    Matrix getInterpolatedTensor( int &id, float &inx, float &iny, float &inz, float &dirX, float &dirY, float &dirZ );

    std::vector<Matrix>* testAngle( int &id, float &dirX, float &dirY, float &dirZ );

    int m_id;

    std::vector<float>* m_mask;
    std::vector<std::vector<Matrix>*>m_logTensors;
    std::vector<std::vector<QVector3D>*> m_evecs;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    int m_blockSize;

    unsigned int m_minLength;
    float m_stepSize;
    float m_diag;
    int maxStepsInVoxel;
    float m_smoothness;

    std::vector<Fib>m_fibs;


signals:
    void progress();
    void finished();

private:
    int getID( float x, float y, float z )
    {
        int id = (int) ( x / m_dx ) + (int) ( y / m_dy ) * m_nx + (int) ( z / m_dz ) * m_ny * m_nx;

        id = max( (int) 0, min( m_blockSize - 1, id ) );
        return id;
    }

    void getXYZ( int id, int &x, int &y, int &z )
    {
        x = id % m_nx;
        int tempY = id % ( m_nx * m_ny );
        y = tempY / m_nx;
        z = id / ( m_nx * m_ny );
    }
};

#endif /* TWCTHREAD_H_ */
