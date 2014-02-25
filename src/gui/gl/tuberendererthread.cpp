/*
 * tuberendererthread.cpp
 *
 * Created on: 26.02.2012
 * @author Ralph Schurade
 */
#include "tuberendererthread.h"

#include "glfunctions.h"

#include <QVector3D>

#include <math.h>

TubeRendererThread::TubeRendererThread( std::vector<Fib>* fibs, int id ) :
    m_fibs( fibs ),
    m_id( id )
{
    m_verts = new std::vector<float>();
}

TubeRendererThread::~TubeRendererThread()
{
    m_verts->clear();
}

std::vector<float>* TubeRendererThread::getVerts()
{
    return m_verts;
}

void TubeRendererThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;

    int chunkSize = m_fibs->size() / numThreads;

    int begin = m_id * chunkSize;
    int end = m_id * chunkSize + chunkSize;

    if ( m_id == numThreads - 1 )
    {
        end = m_fibs->size();
    }

    // for all voxels:
    for ( int i = begin; i < end; ++i )
    {
        Fib fib = m_fibs->at( i );

        if ( fib.length() < 2 )
        {
            printf( "fib with size < 2 detected" );
            continue;
        }

        QVector3D lineStart = fib.firstVert();
        QVector3D lineEnd = fib.lastVert();

        QVector3D secondVert = fib.getVert( 1 );
        QVector3D localColor( lineStart.x() - secondVert.x(), lineStart.y() - secondVert.y(), lineStart.z() - secondVert.z() );
        localColor.normalize();

        // push back the first vertex, done seperately because of nomal calculation
        m_verts->push_back( lineStart.x() );
        m_verts->push_back( lineStart.y() );
        m_verts->push_back( lineStart.z() );
        m_verts->push_back( localColor.x() );
        m_verts->push_back( localColor.y() );
        m_verts->push_back( localColor.z() );
        m_verts->push_back( 1.0 );

        m_verts->push_back( lineStart.x() );
        m_verts->push_back( lineStart.y() );
        m_verts->push_back( lineStart.z() );
        m_verts->push_back( localColor.x() );
        m_verts->push_back( localColor.y() );
        m_verts->push_back( localColor.z() );
        m_verts->push_back( -1.0 );

        for ( unsigned int k = 1; k < fib.length() - 1; ++k )
        {
            QVector3D localColor( fib[k-1].x() - fib[k+1].x(), fib[k-1].y() - fib[k+1].y(), fib[k-1].z() - fib[k+1].z() );
            localColor.normalize();

            m_verts->push_back( fib[k].x() );
            m_verts->push_back( fib[k].y() );
            m_verts->push_back( fib[k].z() );
            m_verts->push_back( localColor.x() );
            m_verts->push_back( localColor.y() );
            m_verts->push_back( localColor.z() );
            m_verts->push_back( 1.0 );

            m_verts->push_back( fib[k].x() );
            m_verts->push_back( fib[k].y() );
            m_verts->push_back( fib[k].z() );
            m_verts->push_back( localColor.x() );
            m_verts->push_back( localColor.y() );
            m_verts->push_back( localColor.z() );
            m_verts->push_back( -1.0 );
        }

        QVector3D sec2last = fib[ fib.length() - 2 ];
        QVector3D localColor2( sec2last.x() - lineEnd.x(), sec2last.y() - lineEnd.y(), sec2last.z() - lineEnd.z() );
        localColor.normalize();

        // push back the last vertex, done seperately because of nomal calculation
        m_verts->push_back( lineEnd.x() );
        m_verts->push_back( lineEnd.y() );
        m_verts->push_back( lineEnd.z() );
        m_verts->push_back( localColor2.x() );
        m_verts->push_back( localColor2.y() );
        m_verts->push_back( localColor2.z() );
        m_verts->push_back( 1.0 );

        m_verts->push_back( lineEnd.x() );
        m_verts->push_back( lineEnd.y() );
        m_verts->push_back( lineEnd.z() );
        m_verts->push_back( localColor2.x() );
        m_verts->push_back( localColor2.y() );
        m_verts->push_back( localColor2.z() );
        m_verts->push_back( -1.0 );
    }
}
