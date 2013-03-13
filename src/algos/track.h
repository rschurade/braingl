/*
 * track.h
 *
 *  Created on: Dec 11, 2012
 *      Author: Ralph Schurade
 */

#ifndef TRACK_H_
#define TRACK_H_

#include "fmath.h"

#include <QDebug>
#include <QVector>
#include <QVector3D>

class DatasetTensor;
class TrackThread;

class Track : public QObject
{
    Q_OBJECT

public:
    Track( DatasetTensor* ds );
    virtual ~Track();

    void startTracking();

    QVector< QVector< float > >getFibs();
    QVector< QVector< float > >getExtras();
    int getNumPoints();
    int getNumLines();

private:
    void trackWholeBrain();

    DatasetTensor* m_dataset;

    QVector<Matrix>* m_tensors;
    QVector<Matrix>* m_logTensors;
    // calculated fa and eigen vectors
    QVector<float> m_fa;
    QVector<QVector3D> m_evec1;

    QVector<TrackThread*> m_threads;

    QVector< QVector< float > >fibs;
    QVector< QVector< float > >extras;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    int blockSize;

    // user defineable parameters for tracking
    int m_minLength;
    float m_minFA;
    float m_minStartFA;
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
    void setMinFA( float value, int );
    void setMinStartFA( float value, int );
    void setStepSize( float value, int );
    void setSmoothness( float value, int );

signals:
    void progress();
    void finished();
};

#endif /* TRACK_H_ */
