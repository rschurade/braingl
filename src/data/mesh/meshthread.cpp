/*
 * meshthread.cpp
 *
 *  Created on: 29.12.2012
 *      Author: Ralph
 */

#include "meshthread.h"

MeshThread::MeshThread( QVector<float>* vertices, QVector<int>* triangles, int numTris, int id ) :
    m_vertices( vertices ),
    m_triangles( triangles ),
    m_numTris( numTris ),
    m_id( id )
{
}

MeshThread::~MeshThread()
{
}

QVector<QVector3D>* MeshThread::getTriNormals()
{
    return &m_triNormals;
}

void MeshThread::run()
{
    int numThreads = QThread::idealThreadCount();

    int chunkSize = m_numTris / numThreads;

    int begin = m_id * chunkSize;
    int end = m_id * chunkSize + chunkSize;

    if ( m_id == numThreads - 1 )
    {
        end = m_numTris;
    }

    m_triNormals.resize( end - begin  );

    float v1x, v1y, v1z;
    float v2x, v2y, v2z;

    for ( int i = begin; i < end; ++i )
    {
        v1x = m_vertices->at( 6 * m_triangles->at( 3*i+1 ) ) - m_vertices->at( 6 * m_triangles->at( 3*i ) );
        v1y = m_vertices->at( 6 * m_triangles->at( 3*i+1 ) +1 ) - m_vertices->at( 6 * m_triangles->at( 3*i)+1);
        v1z = m_vertices->at( 6 * m_triangles->at( 3*i+1)+2 ) - m_vertices->at( 6 * m_triangles->at( 3*i)+2);

        v2x = m_vertices->at( 6 * m_triangles->at( 3*i+2)  ) - m_vertices->at( 6 * m_triangles->at( 3*i)  );
        v2y = m_vertices->at( 6 * m_triangles->at( 3*i+2)+1) - m_vertices->at( 6 * m_triangles->at( 3*i)+1);
        v2z = m_vertices->at( 6 * m_triangles->at( 3*i+2)+2) - m_vertices->at( 6 * m_triangles->at( 3*i)+2);

        m_triNormals[i - begin].setX( v1y * v2z - v1z * v2y );
        m_triNormals[i - begin].setY( v1z * v2x - v1x * v2z );
        m_triNormals[i - begin].setZ( v1x * v2y - v1y * v2x );

        m_triNormals[i - begin].normalize();
    }
}
