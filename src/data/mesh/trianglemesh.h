/*
 * trianglemesh.h
 *
 *  Created on: Jun 4, 2012
 *      Author: schurade
 */

#ifndef TRIANGLEMESH_H_
#define TRIANGLEMESH_H_

#include <QtCore/QVector>
#include <QtGui/QVector3D>
#include <QtGui/QColor>

struct Triangle
{
    int v0;
    int v1;
    int v2;
};

class TriangleMesh
{
public:
    TriangleMesh( int vertSize = 0, int triSize = 0 );
    virtual ~TriangleMesh();

    int getVertSize(); //!< returns the number of vertices in the mesh
    int getTriSize(); //!< returns the number of triangles in the mesh

    void addVertex( int index, QVector3D position, QColor color = QColor( 255, 255, 255 ) );
    void addVertex( QVector3D position, QColor color = QColor( 255, 255, 255 ) );

    void addTriangle( int index, Triangle triangle, QColor color = QColor( 255, 255, 255 ) );
    void addTriangle( Triangle triangle, QColor color = QColor( 255, 255, 255 ) );

    void removeTriangle( int index );
    void removeVertex( int index );

private:
    QVector3D calcTriNormal( Triangle triangle );
    QVector3D calcVertNormal( int vertex );

    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_vertNormals;
    QVector<QColor>    m_vertColors;

    QVector<Triangle>  m_triangles;
    QVector<QVector3D> m_triNormals;
    QVector<QColor>    m_triColors;

    int m_reservedForVerts;
    int m_reservedForTris;
};

#endif /* TRIANGLEMESH_H_ */