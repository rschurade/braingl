/*
 * trianglemesh2.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include "meshthread.h"
#include "trianglemesh2.h"

TriangleMesh2::TriangleMesh2( int numVerts, int numTris ) :
    m_numVerts( numVerts ),
    m_numTris( numTris ),
    m_vertexInsertId( 0 ),
    m_triangleInsertId( 0 )
{
    m_vertices.resize( numVerts * 6 );
    m_colors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );

    m_numThreads = QThread::idealThreadCount();

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, i ) );
    }
}

TriangleMesh2::~TriangleMesh2()
{
}

void TriangleMesh2::finalize()
{
    calcTriNormals();
    calcVertNormals();
}

void TriangleMesh2::addVertex( int id, float x, float y, float z )
{
    m_vertices[ id * 6     ] = x;
    m_vertices[ id * 6 + 1 ] = y;
    m_vertices[ id * 6 + 2 ] = z;
}

void TriangleMesh2::addVertex( float x, float y, float z )
{
    m_vertices[ m_vertexInsertId++ ] = x;
    m_vertices[ m_vertexInsertId++ ] = y;
    m_vertices[ m_vertexInsertId++ ] = z;
    m_vertexInsertId += 3;
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

float* TriangleMesh2::getVertices()
{
    return m_vertices.data();
}

int* TriangleMesh2::getIndexes()
{
    return m_triangles.data();
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
        m_vertices[ 6 * i + 3 ] = sum.x();
        m_vertices[ 6 * i + 4 ] = sum.y();
        m_vertices[ 6 * i + 5 ] = sum.z();
    }
}
