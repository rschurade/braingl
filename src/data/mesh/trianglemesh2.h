/*
 * trianglemesh2.h
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */

#ifndef TRIANGLEMESH2_H_
#define TRIANGLEMESH2_H_

#include <QtCore/QVector>
#include <QtGui/QVector3D>

class MeshThread;

class TriangleMesh2
{
public:
    TriangleMesh2( int numVerts, int numTris );
    virtual ~TriangleMesh2();

    void addVertex( int id, float x, float y, float z );
    void addTriangle( int id, int v0, int v1, int v2 );

    void finalize();

    float* getVertices();
    int* getIndexes();

    int numVerts() { return m_numVerts; };
    int numTris() { return m_numTris; };

private:
    void calcTriNormals();
    void calcVertNormals();

    int m_numVerts;
    int m_numTris;

    QVector<float>m_vertices;
    QVector<float>m_normals;
    QVector<float>m_colors;
    QVector<QVector<int> >m_vertIsInTriangle;
    QVector<QVector<int> >m_vertNeighbors;

    QVector<int>m_triangles;
    QVector<QVector3D>m_triNormals;

    int m_numThreads;
    QVector<MeshThread*> m_threads;
};

#endif /* TRIANGLEMESH2_H_ */
