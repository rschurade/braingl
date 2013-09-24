/*
 * connections.h
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

#include "edge.h"

#include "../data/datasets/datasetfibers.h"

#include <QString>
#include <QList>
#include <QVector3D>

class Connections {
public:
	Connections();
    Connections(QString nname, QString ename);
    Connections(QString fib);
	virtual ~Connections();

    void params();
    void subdivide();
    void subdivide(int newp);
    void attract();

    void fullAttract();
    void calcComps();
    float* comps;
    float comp(int i, int j);
    void writeVTK();
    void writeBinaryVTK();
    void writeBinaryVTK(QString name);
    void writeSegments();
    void loadConnexels(QString filename);
    void loadFib(QString fib);
    QString name();

    QList<Dataset*> createDatasetFibers();

    double c_thr, bell;
    int start_i, numcycles, smooth;
    QString prefix;

    QList<QVector3D> nodes;
    QList<Edge*> edges;
    double vis_c(Edge* e1, Edge* e2);
    QVector3D proj(QVector3D a, QVector3D b, QVector3D p);
};

#endif /* CONNECTIONS_H_ */
