/*
 * edge.h
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#ifndef EDGE_H_
#define EDGE_H_

#include <QList>
#include <QVector3D>

class Edge
{
public:
    Edge();
    virtual ~Edge();
    Edge(QVector3D fn, QVector3D tn, float value=1.0);
    QVector3D fn, tn;
    QList<QVector3D> points;
    QList<QVector3D> forces;

    void subdivide(int newp);
    void attract();
    void applyForces();
    bool flip(Edge* other);
    double length();
    double segLength(int n);
    float m_value;
};

#endif /* EDGE_H_ */
