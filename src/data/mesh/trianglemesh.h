/*
 * trianglemesh.h
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#ifndef TRIANGLEMESH_H_
#define TRIANGLEMESH_H_


#include "point.h"
#include "triangle.h"

#include <QMap>
#include <QVector>

class TriangleMesh
{
public:
    TriangleMesh();
    virtual ~TriangleMesh();

    void addVertex( Point point );
    void addVertex( QVector3D point );
    void addVertex( float x, float y, float z );

    void addTriangle( Triangle tri );
    void addTriangle( int p0, int p1, int p2 );

private:
    int m_vertexInsertId;

    QMap<int, Point> m_points;
    QVector<Triangle> m_triangles;

};

#endif /* TRIANGLEMESH_H_ */
