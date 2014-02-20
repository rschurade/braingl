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
    TriangleMesh2( unsigned int numVerts, unsigned int numTris );
    TriangleMesh2( TriangleMesh2* trim );
    virtual ~TriangleMesh2();

    void resize( unsigned int numVerts, unsigned int numTris );

    void setVertex( unsigned int id, float x, float y, float z );
    void setVertex( unsigned int id, QVector3D pos );
    bool addVertex( float x, float y, float z );

    void setTriangle( unsigned int id, unsigned int v0, unsigned int v1, unsigned int v2 );
    void setTriangle( unsigned int id, Triangle tri );
    void addTriangle( unsigned int v0, unsigned int v1, unsigned int v2 );
    void addTriangle( Triangle tri );

    void setVertexColor( unsigned int id, QColor color );
    void setVertexColor( unsigned int id, float r, float g, float b, float a );

    void setVertexData( unsigned int id, float value );
    float getVertexData( unsigned int id );

    std::vector<unsigned int> getStar( unsigned int id );
    unsigned int getNextVertex( unsigned int triNum, unsigned int vertNum );
    unsigned int getNeighbor( unsigned int coVert1, unsigned int coVert2, unsigned int triangleNum );
    unsigned int getThirdVert( unsigned int coVert1, unsigned int coVert2, unsigned int triangleNum );

    void finalize();

    float* getVertices();
    QColor getVertexColor( unsigned int i );
    float* getVertexColors();
    unsigned int* getIndexes();

    unsigned int numVerts() { return m_numVerts; };
    unsigned int numTris() { return m_numTris; };

    unsigned int currentNumVerts() { return m_vertexInsertId / m_bufferSize; };
    unsigned int currenNumTris() { return m_triangleInsertId / 3; };

    unsigned int bufferSize();

    std::vector<unsigned int> pick( QVector3D pos, float radius );
    unsigned int closestVertexIndex( QVector3D pos );

    QVector3D getVertex( unsigned int id );
    QVector3D getVertexNormal( unsigned int id );
    std::vector<unsigned int> getTriangle( unsigned int id );
    Triangle getTriangle2( unsigned int id );

    std::vector<unsigned int> getTriangles() { return m_triangles; };

private:
    void calcTriNormals();
    void calcVertNormals();

    void buildOcTree();
    void collapseVertex( unsigned int toId, unsigned int toRemoveId );

    unsigned int m_bufferSize;

    unsigned int m_numVerts;
    unsigned int m_numTris;

    std::vector<float>m_vertices;
    std::vector<float>m_vertexColors;

    std::vector<std::vector<unsigned int> >m_vertIsInTriangle;
    std::vector<std::vector<unsigned int> >m_vertNeighbors;

    std::vector<unsigned int>m_triangles;
    std::vector<QVector3D>m_triNormals;

    int m_numThreads;
    std::vector<MeshThread*> m_threads;

    unsigned int m_vertexInsertId;
    unsigned int m_colorInsertId;
    unsigned int m_triangleInsertId;

    OcTree* m_ocTree;
    std::vector<unsigned int>m_toRemove;
};

#endif /* TRIANGLEMESH2_H_ */
