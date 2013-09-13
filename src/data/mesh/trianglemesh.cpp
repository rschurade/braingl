/*
 * trianglemesh.cpp
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#include "trianglemesh.h"

TriangleMesh::TriangleMesh() :
    m_vertexInsertId( 0 )
{
}

TriangleMesh::~TriangleMesh()
{
}

void TriangleMesh::addVertex( Point point )
{
    m_points.insert( m_vertexInsertId++, point );
}

void TriangleMesh::addVertex( QVector3D point )
{
    Point p( point );
    addVertex( p );
}

void TriangleMesh::addVertex( float x, float y, float z )
{
    Point p( QVector3D( x, y, z ) );
    addVertex( p );
}

void TriangleMesh::addTriangle( Triangle tri )
{
    m_triangles.push_back( tri );
//    m_points[ tri.v0 ].inTriangle.insert( m_triangles.size() - 1 );
//    m_points[ tri.v1 ].inTriangle.insert( m_triangles.size() - 1 );
//    m_points[ tri.v2 ].inTriangle.insert( m_triangles.size() - 1 );
}

void TriangleMesh::addTriangle( int p0, int p1, int p2 )
{
    Triangle t( p0, p1, p2 );
    addTriangle( t );
}
