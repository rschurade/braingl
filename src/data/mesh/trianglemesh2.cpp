/*
 * trianglemesh2.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */
#include "trianglemesh2.h"

TriangleMesh2::TriangleMesh2( int numVerts, int numTris ) :
    m_numVerts( numVerts ),
    m_numTris( numTris )
{
    m_vertices.resize( numVerts * 6 );
    m_colors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );
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

void TriangleMesh2::addTriangle( int id, int v0, int v1, int v2 )
{
    m_triangles[ id * 3     ] = v0;
    m_triangles[ id * 3 + 1 ] = v1;
    m_triangles[ id * 3 + 2 ] = v2;

    m_vertIsInTriangle[v0].push_back( id );
    m_vertIsInTriangle[v1].push_back( id );
    m_vertIsInTriangle[v2].push_back( id );
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
    float v1x, v1y, v1z;
    float v2x, v2y, v2z;

    for ( int i = 0; i < m_numTris; ++i )
    {
        v1x = m_vertices[6 * m_triangles[3*i+1]  ] - m_vertices[6 * m_triangles[3*i]  ];
        v1y = m_vertices[6 * m_triangles[3*i+1]+1] - m_vertices[6 * m_triangles[3*i]+1];
        v1z = m_vertices[6 * m_triangles[3*i+1]+2] - m_vertices[6 * m_triangles[3*i]+2];

        v2x = m_vertices[6 * m_triangles[3*i+2]  ] - m_vertices[6 * m_triangles[3*i]  ];
        v2y = m_vertices[6 * m_triangles[3*i+2]+1] - m_vertices[6 * m_triangles[3*i]+1];
        v2z = m_vertices[6 * m_triangles[3*i+2]+2] - m_vertices[6 * m_triangles[3*i]+2];

        m_triNormals[i].setX( v1y * v2z - v1z * v2y );
        m_triNormals[i].setY( v1z * v2x - v1x * v2z );
        m_triNormals[i].setZ( v1x * v2y - v1y * v2x );

        m_triNormals[i].normalize();
    }
}

void TriangleMesh2::calcVertNormals()
{
    QVector3D sum( 0, 0, 0 );
    for ( int i = 0; i < m_numVerts; ++i )
    {
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
