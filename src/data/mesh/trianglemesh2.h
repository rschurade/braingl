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

struct Point {
    int newID;
    QVector3D pos;
};

struct Triangle {
    int v0;
    int v1;
    int v2;
};

class TriangleMesh2
{
public:
    TriangleMesh2( int numVerts, int numTris );
    TriangleMesh2( TriangleMesh2* trim );
    virtual ~TriangleMesh2();

    void resize( int numVerts, int numTris );

    void setVertex( int id, float x, float y, float z );
    void setVertex( int id, QVector3D pos );
    bool addVertex( float x, float y, float z );

    void setTriangle( int id, int v0, int v1, int v2 );
    void setTriangle( int id, Triangle tri );
    void addTriangle( int v0, int v1, int v2 );
    void addTriangle( Triangle tri );

    void setVertexColor( int id, QColor color );
    void setVertexColor( int id, float r, float g, float b, float a );

    void setVertexData( int id, float value );
    float getVertexData( int id );

    QVector<int> getStar( int id );
    int getNextVertex( int triNum, int vertNum );
    int getNeighbor( int coVert1, int coVert2, int triangleNum );
    int getThirdVert( int coVert1, int coVert2, int triangleNum );

    void finalize();

    float* getVertices();
    QColor getVertexColor(int i);
    float* getVertexColors();
    int* getIndexes();

    int numVerts() { return m_numVerts; };
    int numTris() { return m_numTris; };

    int currentNumVerts() { return m_vertexInsertId / m_bufferSize; };
    int currenNumTris() { return m_triangleInsertId / 3; };

    int bufferSize();

    QVector<int> pick( QVector3D pos, float radius );
    int closestVertexIndex( QVector3D pos );

    QVector3D getVertex(int id);
    QVector3D getVertexNormal(int id);
    QVector<int> getTriangle( int id );
    Triangle getTriangle2( int id );

    QVector<int> getTriangles() { return m_triangles; };

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
