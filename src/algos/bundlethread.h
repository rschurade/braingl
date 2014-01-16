/*
 * bundlethread.h
 *
 * Created on: 04.11.2013
 * @author Ralph Schurade
 */

#ifndef BUNDLETHREAD_H_
#define BUNDLETHREAD_H_

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QVector3D>

class KdTree;

class BundleThread : public QThread
{
    Q_OBJECT

public:
    BundleThread( int id, QVector< std::vector<float> >* fibs, KdTree* kdTree, std::vector<float>* kdVerts, QVector<int>* revInd, int numPoints );

    virtual ~BundleThread();

    QVector< QVector3D >getForces();
    void setRadius( float value ) { m_radius = value; };
    void setIterations( float value ) { m_iterations = value; };

private:
    void run();

    void calculateForces();

    void boxTest( QVector<QVector3D>& workfield, QVector<int>& revInds, int left, int right, int axis );

    int m_id;

    QVector< std::vector<float> >* m_fibs;
    QVector< QVector3D > m_forces;
    KdTree* m_kdTree;
    std::vector<float>* m_kdVerts;
    QVector<int>* m_revInd;

    std::vector<float> m_boxMin;
    std::vector<float> m_boxMax;
    float m_radius;
    int m_numPoints;
    float m_iterations;

signals:
    void progress();
    void finished();
};

#endif /* BUNDLETHREAD_H_ */
