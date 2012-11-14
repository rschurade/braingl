/*
 * trianglemesh.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: schurade
 */

#include "trianglemesh.h"

#include <QtCore/QDebug>

TriangleMesh::TriangleMesh(  int vertSize, int triSize ) :
    m_dirty( true ),
    m_reservedForVerts( 50 ),
    m_reservedForTris( 50 )
{
    if ( vertSize > 0 )
    {
        m_vertices.resize( vertSize );
        m_vertNormals.resize( vertSize );
        m_vertColors.resize( vertSize );
        m_neighbors.resize( vertSize );
    }
    else
    {
        m_vertices.reserve( m_reservedForVerts );
        m_vertNormals.reserve( m_reservedForVerts );
        m_vertColors.reserve( m_reservedForVerts );
        m_neighbors.reserve( m_reservedForVerts );
    }
    if ( triSize > 0 )
    {
        m_triangles.resize( triSize );
        m_triNormals.resize( triSize );
        m_triColors.resize( triSize );
    }
    else
    {
        m_triangles.reserve( m_reservedForTris );
        m_triNormals.reserve( m_reservedForTris );
        m_triColors.reserve( m_reservedForTris );
    }
}

TriangleMesh::~TriangleMesh()
{
}

int TriangleMesh::getVertSize()
{
    return m_vertices.size();
}

int TriangleMesh::getTriSize()
{
    return m_triangles.size();
}

void TriangleMesh::addVertex( int index, QVector3D position, QColor color )
{
    if ( index >= 0 && index < m_vertices.size() )
    {
        m_vertices[ index ] = position;
        m_vertColors[ index ] = color;
    }
    m_dirty = true;
}

void TriangleMesh::addVertex( int index, float x, float y, float z, QColor color )
{
    addVertex( index, QVector3D( x, y, z), color );
}

void TriangleMesh::addVertex( QVector3D position, QColor color )
{
    m_vertices.push_back( position );
    m_vertNormals.push_back( position );
    m_vertColors.push_back( color );

    if ( m_vertices.size() > m_reservedForVerts / 2 )
    {
        m_vertices.reserve( m_reservedForVerts );
        m_vertNormals.reserve( m_reservedForVerts );
        m_vertColors.reserve( m_reservedForVerts );
    }
    m_dirty = true;
}

void TriangleMesh::addVertex( float x, float y, float z )
{
    addVertex( QVector3D( x, y,z ) );
}

void TriangleMesh::addTriangle( int index, Triangle triangle, QColor color )
{
    if ( index >= 0 && index < m_triangles.size() )
    {
        m_triangles[index] = triangle;
        m_triColors[index] = color;
        m_triNormals[index] = calcTriNormal( triangle );
    }
    m_dirty = true;
}

void TriangleMesh::addTriangle( int index, int id0, int id1, int id2, QColor color )
{
    addTriangle( index, Triangle( { id0, id1, id2 } ), color );
}

void TriangleMesh::addTriangle( Triangle triangle, QColor color )
{
    m_triangles.push_back( triangle );
    m_triColors.push_back( color );
    m_triNormals.push_back( calcTriNormal( triangle ) );

    if ( m_triangles.size() > m_reservedForTris / 2 )
    {
        m_triangles.reserve( m_reservedForTris );
        m_triNormals.reserve( m_reservedForTris );
        m_triColors.reserve( m_reservedForTris );
    }
    m_dirty = true;
}

void TriangleMesh::addTriangle( int id0, int id1, int id2 )
{
    addTriangle( Triangle( { id0, id1, id2 } ) );
}

QVector3D TriangleMesh::calcTriNormal( Triangle triangle )
{
    QVector3D v1( m_vertices[triangle.v1] - m_vertices[triangle.v0] );
    QVector3D v2( m_vertices[triangle.v2] - m_vertices[triangle.v0] );

    QVector3D tempNormal;

    tempNormal.setX( v1.y() * v2.z() - v1.z() * v2.y() );
    tempNormal.setY( v1.z() * v2.x() - v1.x() * v2.z() );
    tempNormal.setZ( v1.x() * v2.y() - v1.y() * v2.x() );

    tempNormal.normalize();

    return tempNormal;
}

QVector3D TriangleMesh::calcVertNormal( int vertex )
{
    QVector3D tempNormal( 0, 0, 0 );
    int count = 0;
    for ( int i = 0; i < m_triangles.size(); ++i )
    {
        if( m_triangles[i].v0 == vertex || m_triangles[i].v1 == vertex || m_triangles[i].v2 == vertex )
        {
            tempNormal += m_triNormals[i];
            ++count;
        }
    }
    return tempNormal / ( ( count > 0 ) ? count : 1 );
}

void TriangleMesh::recalcNormals()
{
    for ( int i = 0; i < m_vertices.size(); ++i )
    {
        m_vertNormals[i] = calcVertNormal(i);
    }
}

void TriangleMesh::recalcNeighbors()
{
    QSet<int> v;
    for ( int i = 0; i < m_vertices.size(); ++i )
    {
        m_neighbors[i] = v;
    }
    for ( int i = 0; i < m_triangles.size(); ++i )
    {
        m_neighbors[ m_triangles[i].v0 ].insert( m_triangles[i].v1 );
        m_neighbors[ m_triangles[i].v0 ].insert( m_triangles[i].v2 );

        m_neighbors[ m_triangles[i].v1 ].insert( m_triangles[i].v0 );
        m_neighbors[ m_triangles[i].v1 ].insert( m_triangles[i].v2 );

        m_neighbors[ m_triangles[i].v2 ].insert( m_triangles[i].v0 );
        m_neighbors[ m_triangles[i].v2 ].insert( m_triangles[i].v1 );
    }
}

void TriangleMesh::removeTriangle( int index )
{
    if ( index >= 0 && index < m_triangles.size() )
    {
        m_triangles.remove( index );
        m_triNormals.remove( index );
        m_triColors.remove( index );
    }
}

void TriangleMesh::removeVertex( int index )
{
    if ( index >= 0 && index < m_vertices.size() )
    {
        for ( int i = 0; i < m_triangles.size(); ++i )
        {
            if( m_triangles[i].v0 == index || m_triangles[i].v1 == index || m_triangles[i].v2 == index )
            {
                removeTriangle( i );
            }
        }
        m_vertices.remove( index );
        m_vertNormals.remove( index );
        m_vertColors.remove( index );
        for ( int i = 0; i < m_triangles.size(); ++i )
        {
            if( m_triangles[i].v0 > index )
            {
                m_triangles[i].v0 -= 1;
            }
            if ( m_triangles[i].v1 > index )
            {
                m_triangles[i].v1 -= 1;
            }
            if ( m_triangles[i].v2 > index )
            {
                m_triangles[i].v2 -= 1;
            }
        }
    }
}

void TriangleMesh::clearDirty()
{
    recalcNormals();
    recalcNeighbors();
    m_dirty = false;
}


QVector< QVector3D >& TriangleMesh::getVertices()
{
    return m_vertices;
}
QVector< QVector3D >& TriangleMesh::getVertNormals()
{
    if ( m_dirty )
    {
        clearDirty();
    }
    return m_vertNormals;
}
QVector< QColor >& TriangleMesh::getVertColors()
{
    return m_vertColors;
}
QVector< Triangle >& TriangleMesh::getTriangles()
{
    return m_triangles;
}

QVector< QSet<int> >& TriangleMesh::getNeighbors()
{
    if ( m_dirty )
    {
        clearDirty();
    }
    return m_neighbors;
}
