/*
 * connections.h
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

#include "edge.h"

#include "bundlingthread.h"
#include "attractthread.h"
#include "compatibilities.h"
#include "compatibilitiesthread.h"

#include "../data/datasets/datasetfibers.h"

#include <QString>
#include <QList>
#include <QVector3D>

class BundlingThread;
class AttractThread;

class Connections: public QObject
{
Q_OBJECT

public:
    Connections();
    Connections( QString nname, QString ename );
    Connections( QString fib );
    virtual ~Connections();

    void params();
    void subdivide();
    void subdivide( int newp );
    void attract();

    void fullAttract();
    void calcComps();
    Compatibilities* compatibilities;
    void writeVTK();
    void writeBinaryVTK();
    void writeBinaryVTK( QString name );
    void writeSegments();
    void loadConnexels( QString filename );
    void loadFib( QString fib );
    QString name();

    QList<Dataset*> createDatasetFibers();

    double c_thr, bell;
    int start_i, numcycles, smooth;
    QString prefix;

    QList<QVector3D> nodes;
    QList<Edge*> edges;
    QMultiHash<QVector3D,Edge*> m_hashed_edges; //containing lists of edges from unique nodes
    double vis_c( Edge* e1, Edge* e2 );
    QVector3D proj( QVector3D a, QVector3D b, QVector3D p );

    void startBundling();

    QVector<AttractThread*> m_athreads;
    int m_athreadsRunning;

    QVector<CompatibilitiesThread*> m_compthreads;
    int m_compthreadsRunning;

    void hashEdges();

private slots:
    void setCthr( float value, int );
    void setBell( float value, int );
    void setSmooth( int value, int );
    void attractThreadFinished();
    void compThreadFinished();
    void compThreadProgress();

signals:
    void progress();
    void finished();

};

#endif /* CONNECTIONS_H_ */
