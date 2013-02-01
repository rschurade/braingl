/*
 * twcthread.h
 *
 *  Created on: Jan 28, 2013
 *      Author: schurade
 */

#ifndef TWCTHREAD_H_
#define TWCTHREAD_H_

#include "../thirdparty/newmat10/newmat.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

class DatasetTensor;

class TWCThread : public QThread
{
    Q_OBJECT

public:
    TWCThread( int id,
                QVector<float>* mask,
                QVector<Matrix>* logtensors1,
                QVector<Matrix>* logtensors2,
                QVector<Matrix>* logtensors3,
                QVector<QVector3D>* evec1,
                QVector<QVector3D>* evec2,
                QVector<QVector3D>* evec3,
                int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~TWCThread();

    QVector< QVector< float > >getFibs();

private:
    void run();

    void track( int id, bool negDir, QVector<float>& result );

    float getInterpolatedFA( int &id, float &inx, float &iny, float &inz );
    Matrix getInterpolatedTensor( int &id, float &inx, float &iny, float &inz, float &dirX, float &dirY, float &dirZ );

    QVector<Matrix>* testAngle( int &id, float &dirX, float &dirY, float &dirZ );

    int m_id;

    QVector<float>* m_mask;
    QVector<QVector<Matrix>*>m_logTensors;
    QVector<QVector<QVector3D>*> m_evecs;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    int m_blockSize;

    int m_minLength;
    float m_stepSize;
    float m_diag;
    int maxStepsInVoxel;
    float m_smoothness;

    QVector< QVector< float > >fibs;

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
