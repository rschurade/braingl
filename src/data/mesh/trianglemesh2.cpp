/*
 * trianglemesh2.cpp
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */
#include "trianglemesh2.h"
#include "meshthread.h"
#include "octree.h"

#include "../../gui/gl/glfunctions.h"

#include <QDebug>

TriangleMesh2::TriangleMesh2( int numVerts, int numTris ) :
    m_bufferSize( 7 ),
    m_numVerts( numVerts ),
    m_numTris( numTris ),
    m_vertexInsertId( 0 ),
    m_colorInsertId( 0 ),
    m_triangleInsertId( 0 ),
    m_ocTree( 0 )
{
    m_vertices.resize( numVerts * m_bufferSize );
    m_vertexColors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }
}

TriangleMesh2::TriangleMesh2( TriangleMesh2* trim ) :
    m_bufferSize( 7 ),
    m_numVerts( trim->numVerts() ),
    m_numTris( trim->numTris() ),
    m_vertexInsertId( 0 ),
    m_colorInsertId( 0 ),
    m_triangleInsertId( 0 ),
    m_ocTree( 0 )
{
    m_vertices.resize( trim->numVerts() * m_bufferSize );
    m_vertexColors.resize( trim->numVerts() * 4 );
    m_vertIsInTriangle.resize( trim->numVerts() );
    m_vertNeighbors.resize( trim->numVerts() );

    m_triangles.resize( trim->numTris() * 3 );
    m_triNormals.resize( trim->numTris() );

    m_numThreads = GLFunctions::idealThreadCount;

    m_triangleInsertId = m_numTris * 3;
    m_vertexInsertId = m_numVerts * m_bufferSize;
    m_colorInsertId = m_numVerts * 4;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }

    for ( int i = 0; i < trim->numVerts(); ++i )
    {
        QVector3D v = trim->getVertex( i );
        this->addVertex( i, v.x(), v.y(), v.z() );
    }
    QVector<int> tris = trim->getTriangles();
    for ( int i = 0; i < trim->numTris(); ++i )
    {
        this->addTriangle( i, tris[i * 3], tris[i * 3 + 1], tris[i * 3 + 2] );
    }
    finalize();
}

TriangleMesh2::~TriangleMesh2()
{
    m_vertices.clear();
    m_vertexColors.clear();
    m_vertIsInTriangle.clear();
    m_vertNeighbors.clear();
    m_triangles.clear();
    m_triNormals.clear();
    m_threads.clear();
    m_toRemove.clear();

    m_vertices.squeeze();
    m_vertexColors.squeeze();
    m_vertIsInTriangle.squeeze();
    m_vertNeighbors.squeeze();
    m_triangles.squeeze();
    m_triNormals.squeeze();
    m_threads.squeeze();
    m_toRemove.squeeze();
}

void TriangleMesh2::resize( int numVerts, int numTris )
{
    m_vertices.resize( numVerts * m_bufferSize );
    m_vertexColors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_numVerts = numVerts;

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );

    m_numTris = numTris;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        delete m_threads[i];
    }
    m_threads.clear();
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }
}

int TriangleMesh2::bufferSize()
{
    return m_bufferSize;
}

void TriangleMesh2::finalize()
{
//    if ( m_ocTree == 0 )
//    {
//        buildOcTree();
//    }

    calcTriNormals();
    calcVertNormals();
    //qDebug() << m_numVerts << "vertices";
}

void TriangleMesh2::addVertex( int id, float x, float y, float z )
{
    m_vertices[ id * m_bufferSize     ] = x;
    m_vertices[ id * m_bufferSize + 1 ] = y;
    m_vertices[ id * m_bufferSize + 2 ] = z;

    m_vertexColors[ id * 4     ] = 1.0;
    m_vertexColors[ id * 4 + 1 ] = 1.0;
    m_vertexColors[ id * 4 + 2 ] = 1.0;
    m_vertexColors[ id * 4 + 3 ] = 1.0;
}

void TriangleMesh2::addVertex( int id, QVector3D pos )
{
    addVertex( id, pos.x(), pos.y(), pos.z() );
}

void TriangleMesh2::addVertex( float x, float y, float z )
{
    m_vertices[ m_vertexInsertId++ ] = x;
    m_vertices[ m_vertexInsertId++ ] = y;
    m_vertices[ m_vertexInsertId++ ] = z;

    m_vertexInsertId += 4;

    m_vertexColors[ m_colorInsertId++ ] = 1.0;
    m_vertexColors[ m_colorInsertId++ ] = 1.0;
    m_vertexColors[ m_colorInsertId++ ] = 1.0;
    m_vertexColors[ m_colorInsertId++ ] = 1.0;
}

void TriangleMesh2::addTriangle( int id, int v0, int v1, int v2 )
{
    m_triangles[ id * 3     ] = v0;
    m_triangles[ id * 3 + 1 ] = v1;
    m_triangles[ id * 3 + 2 ] = v2;

    m_vertIsInTriangle[v0].push_back( id );
    m_vertIsInTriangle[v1].push_back( id );
    m_vertIsInTriangle[v2].push_back( id );
}

void TriangleMesh2::addTriangle( int v0, int v1, int v2 )
{
    int id = m_triangleInsertId / 3;
    m_vertIsInTriangle[v0].push_back( id );
    m_vertIsInTriangle[v1].push_back( id );
    m_vertIsInTriangle[v2].push_back( id );

    m_triangles[ m_triangleInsertId++ ] = v0;
    m_triangles[ m_triangleInsertId++ ] = v1;
    m_triangles[ m_triangleInsertId++ ] = v2;
}

void TriangleMesh2::setVertexColor( int id, QColor color )
{
    setVertexColor( id, color.redF(), color.greenF(), color.blueF(), color.alphaF() );
}

void TriangleMesh2::setVertexColor( int id, float r, float g, float b, float a )
{
    m_vertexColors[ id * 4     ] = r;
    m_vertexColors[ id * 4 + 1 ] = g;
    m_vertexColors[ id * 4 + 2 ] = b;
    m_vertexColors[ id * 4 + 3 ] = a;
}

QColor TriangleMesh2::getVertexColor( int id )
{
    float r = m_vertexColors[ id * 4     ];
    float g = m_vertexColors[ id * 4 + 1 ];
    float b = m_vertexColors[ id * 4 + 2 ];
    float a = m_vertexColors[ id * 4 + 3 ];
    return QColor::fromRgbF(r,g,b,a);
}

void TriangleMesh2::setVertexData( int id, float value )
{
    m_vertices[ id * m_bufferSize + 6 ] = value;
}

float TriangleMesh2::getVertexData( int id )
{
    return m_vertices[ id * m_bufferSize + 6 ];
}

float* TriangleMesh2::getVertices()
{
    return m_vertices.data();
}

float* TriangleMesh2::getVertexColors()
{
    return m_vertexColors.data();
}

int* TriangleMesh2::getIndexes()
{
    return m_triangles.data();
}


QVector3D TriangleMesh2::getVertex(int id)
{
    float x = m_vertices[id * m_bufferSize];
    float y = m_vertices[id * m_bufferSize + 1];
    float z = m_vertices[id * m_bufferSize + 2];
    return QVector3D(x,y,z);
}

QVector<int> TriangleMesh2::getTriangle( int id )
{
    QVector<int> out;
    out.push_back( m_triangles[id * 3] );
    out.push_back( m_triangles[id * 3 + 1] );
    out.push_back( m_triangles[id * 3 + 2] );
    return out;
}

QVector<int> TriangleMesh2::getStar( int id )
{
    return m_vertIsInTriangle[id];
}

int TriangleMesh2::getNextVertex( int triNum, int vertNum )
{
    int answer = -1;

    if ( m_triangles[triNum * 3] == vertNum )
    {
        answer = m_triangles[triNum * 3 + 1];
    }
    else if ( m_triangles[triNum * 3 + 1] == vertNum )
    {
        answer = m_triangles[triNum * 3 + 2];
    }
    else
    {
        answer = m_triangles[triNum * 3];
    }

    return answer;
}

void TriangleMesh2::calcTriNormals()
{
    // run threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->wait();
    }
}

void TriangleMesh2::calcVertNormals()
{
    m_triNormals.clear();
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_triNormals += *( m_threads[i]->getTriNormals() );
    }


    for ( int i = 0; i < m_numVerts; ++i )
    {
        QVector3D sum( 0, 0, 0 );
        for ( int k = 0; k < m_vertIsInTriangle[i].size(); ++k )
        {
            sum += m_triNormals[m_vertIsInTriangle[i][k]];
        }
        sum.normalize();
        m_vertices[ m_bufferSize * i + 3 ] = sum.x();
        m_vertices[ m_bufferSize * i + 4 ] = sum.y();
        m_vertices[ m_bufferSize * i + 5 ] = sum.z();
    }
}

void TriangleMesh2::buildOcTree()
{
    qDebug() << "start building OcTree";

    m_ocTree = new OcTree( QVector3D( 0, 0, 0), 512 );
    for( int i = 0; i < m_numVerts; ++i )
    {
        m_ocTree->insert( i, QVector3D( m_vertices[ m_bufferSize * i], m_vertices[ m_bufferSize * i + 1], m_vertices[ m_bufferSize * i + 2] ) );
        /*
        int id = m_ocTree->insert( i, QVector3D( m_vertices[ m_bufferSize * i], m_vertices[ m_bufferSize * i + 1], m_vertices[ m_bufferSize * i + 2] ) );

        if ( id != -1 )
        {
            collapseVertex( id, i );
        }
        */
    }
    //qDebug() << "collapsed" << m_toRemove.size() << "vertexes";
    qDebug() << "end building OcTree";
}

void TriangleMesh2::collapseVertex( int toId, int toRemoveId )
{
    m_toRemove.push_back( toRemoveId );
    for( int i = 0; i < m_triangles.size(); ++i )
    {
        if( m_triangles[i] == toRemoveId )
        {
            m_triangles[i] = toId;
        }
    }
}

QVector<int> TriangleMesh2::pick( QVector3D pos, float radius )
{
    QVector<int> result;

    for ( int i = 0; i < m_numVerts; ++i )
    {
        float d = ( QVector3D( m_vertices[i*m_bufferSize], m_vertices[i*m_bufferSize+1], m_vertices[i*m_bufferSize+2]) - pos ).lengthSquared();
        if ( d < radius )
        {
            result.push_back( i );
        }
    }

    return result;
}

int TriangleMesh2::closestVertexIndex( QVector3D pos )
{
    int closest = -1;
    double minRadius = 10000000;
    for ( int i = 0; i < m_numVerts; ++i )
    {
        float d = ( QVector3D( m_vertices[i * m_bufferSize], m_vertices[i * m_bufferSize + 1], m_vertices[i * m_bufferSize + 2] ) - pos ).lengthSquared();
        if ( d < minRadius )
        {
            minRadius = d;
            closest = i;
        }
    }
    return closest;
}

int TriangleMesh2::getNeighbor( int coVert1, int coVert2, int triangleNum )
{
    QVector<int>candidates = m_vertIsInTriangle[coVert1];
    QVector<int>compares   = m_vertIsInTriangle[coVert2];

    for ( int i = 0; i < candidates.size(); ++i )
    {
        for ( int k = 0; k < compares.size(); ++k )
        {
            if ( ( candidates[i] != triangleNum ) && ( candidates[i] == compares[k] ) )
            {
                return candidates[i];
            }
        }
    }
    return triangleNum;
}
