/*
 * bundle.h
 *
 *  Created on: Nov 4, 2013
 *      Author: schurade
 */

#ifndef BUNDLE_H_
#define BUNDLE_H_

#include <QDebug>
#include <QVector>

class DatasetFibers;
class BundleThread;


class Bundle : public QObject
{
    Q_OBJECT

public:
    Bundle( DatasetFibers* ds );
    virtual ~Bundle();

    QVector< QVector< float > >getFibs();

    void start();
    void startLoop();

    void applyLoopResult();

private:
    DatasetFibers* m_sourceDataset;
    QVector<BundleThread*> m_threads;
    int m_threadsRunning;
    int m_currentLoop;
    int m_iterations;
    float m_radius;
    float m_smoothRange;

    QVector< QVector< float > >m_fibs;
    QVector<float> m_kdVerts;
    QVector<int>m_reverseIndexes;

private slots:
    void slotProgress();
    void slotThreadFinished();
    void setIterations( int value, int );
    void setRadius( float value, int );
    void setSmoothRange( float value, int );

signals:
    void progress();
    void finished();

};

#endif /* BUNDLE_H_ */
