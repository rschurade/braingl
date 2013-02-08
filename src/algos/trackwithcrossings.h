/*
 * trackwithcrossings.h
 *
 *  Created on: Jan 28, 2013
 *      Author: Ralph Schurade
 */

#ifndef TRACKWITHCROSSINGS_H_
#define TRACKWITHCROSSINGS_H_

#include "fmath.h"

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

class DatasetScalar;
class DatasetTensor;
class TWCThread;

class TrackWithCrossings : public QObject
{
    Q_OBJECT

public:
    TrackWithCrossings();
    virtual ~TrackWithCrossings();

    void setDatasets( DatasetScalar* mask, DatasetTensor* ds1, DatasetTensor* ds2, DatasetTensor* ds3 );
    void startTracking();

    QVector< QVector< float > >getFibs();
    int getNumPoints();
    int getNumLines();

private:
    void trackWholeBrain();

    DatasetScalar* m_mask;
    DatasetTensor* m_ds1;
    DatasetTensor* m_ds2;
    DatasetTensor* m_ds3;

    QVector<TWCThread*> m_threads;

    QVector< QVector< float > >fibs;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    int blockSize;

    // user defineable parameters for tracking
    int m_minLength;
    float m_stepSize;
    float m_diag;
    int maxStepsInVoxel;
    float m_smoothness;

    int m_threadsRunning;
    bool m_thinOut;
    int m_numPoints;
    int m_numLines;

private slots:
    void slotProgress();
    void slotThreadFinished();

    void setMinLength( int value, int );
    void setStepSize( float value, int );
    void setSmoothness( float value, int );

signals:
    void progress();
    void finished();

};

#endif /* TRACKWITHCROSSINGS_H_ */
