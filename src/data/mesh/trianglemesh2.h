/*
 * trianglemesh2.h
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */

#ifndef TRIANGLEMESH2_H_
#define TRIANGLEMESH2_H_

#include <QVector>
#include <QVector3D>

class MeshThread;
class OcTree;

class TriangleMesh2
{
public:
    TriangleMesh2( int numVerts, int numTris );
    virtual ~TriangleMesh2();

    void addVertex( int id, float x, float y, float z );
    void addVertex( float x, float y, float z );
    void addTriangle( int id, int v0, int v1, int v2 );
    void addTriangle( int v0, int v1, int v2 );

    void setVertexColor( int id, QColor color );
    void setVertexColor( int id, float r, float g, float b, float a );

    void setVertexData( int id, float value );
    float getVertexData( int id );

    void finalize();

    float* getVertices();
    float* getVertexColors();
    int* getIndexes();

    int numVerts() { return m_numVerts; };
    int numTris() { return m_numTris; };

    int bufferSize();

    QVector<int> pick( QVector3D pos, float radius );



private:
    void calcTriNormals();
    void calcVertNormals();

    void buildOcTree();
    void collapseVertex( int toId, int toRemoveId );

    int m_bufferSize;

    int m_numVerts;
    int m_numTris;

    QVector<float>m_vertices;
    QVector<float>m_vertexColors;

    QVector<QVector<int> >m_vertIsInTriangle;
    QVector<QVector<int> >m_vertNeighbors;

    QVector<int>m_triangles;
    QVector<QVector3D>m_triNormals;

    int m_numThreads;
    QVector<MeshThread*> m_threads;

    int m_vertexInsertId;
    int m_colorInsertId;
    int m_triangleInsertId;

    OcTree* m_ocTree;
    QVector<int>m_toRemove;
};

#endif /* TRIANGLEMESH2_H_ */
