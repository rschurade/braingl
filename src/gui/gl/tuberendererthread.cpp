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

TubeRendererThread::TubeRendererThread( QVector< QVector< float > >& data, QVector< QVector< float > >& extraData, int id ) :
    m_data( data ),
    m_extraData( extraData ),
    m_id( id )
{
    m_verts = new QVector<float>();
}

TubeRendererThread::~TubeRendererThread()
{
    m_verts->clear();
}

QVector<float>* TubeRendererThread::getVerts()
{
    return m_verts;
}


void TubeRendererThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;

    int chunkSize = m_data.size() / numThreads;

    int begin = m_id * chunkSize;
    int end = m_id * chunkSize + chunkSize;

    if ( m_id == numThreads - 1 )
    {
        end = m_data.size();
    }

    // for all voxels:
    for ( int i = begin; i < end; ++i )
    {
        QVector<float> fib = m_data[i];
        QVector<float> extra = m_extraData[i];

        if ( fib.size() < 9 )
        {
            printf( "fib with size < 3 detected" );
            continue;
        }

        int numFloats = fib.size();
        QVector3D lineStart( fib[0], fib[1], fib[2] );
        QVector3D lineEnd( fib[numFloats-3], fib[numFloats-2], fib[numFloats-1] );

        QVector3D globalColor( fabs( lineStart.x() - lineEnd.x() ), fabs( lineStart.y() - lineEnd.y() ), fabs( lineStart.z() - lineEnd.z() ) );
        globalColor.normalize();

        // push back the first vertex, done seperately because of nomal calculation
        QVector3D localColor( fib[0] - fib[3], fib[1] - fib[4], fib[2] - fib[5] );
        localColor.normalize();

        m_verts->push_back( fib[0] );
        m_verts->push_back( fib[1] );
        m_verts->push_back( fib[2] );
        m_verts->push_back( localColor.x() );
        m_verts->push_back( localColor.y() );
        m_verts->push_back( localColor.z() );
        m_verts->push_back( globalColor.x() );
        m_verts->push_back( globalColor.y() );
        m_verts->push_back( globalColor.z() );
        m_verts->push_back( extra.first() );
        m_verts->push_back( 1.0 );

        m_verts->push_back( fib[0] );
        m_verts->push_back( fib[1] );
        m_verts->push_back( fib[2] );
        m_verts->push_back( localColor.x() );
        m_verts->push_back( localColor.y() );
        m_verts->push_back( localColor.z() );
        m_verts->push_back( globalColor.x() );
        m_verts->push_back( globalColor.y() );
        m_verts->push_back( globalColor.z() );
        m_verts->push_back( extra.first() );
        m_verts->push_back( -1.0 );

        for ( int k = 1; k < fib.size() / 3 - 1; ++k )
        {
            QVector3D localColor( fib[k*3-3] - fib[k*3+3], fib[k*3-2] - fib[k*3+4], fib[k*3-1] - fib[k*3+5] );
            localColor.normalize();

            m_verts->push_back( fib[k*3] );
            m_verts->push_back( fib[k*3+1] );
            m_verts->push_back( fib[k*3+2] );
            m_verts->push_back( localColor.x() );
            m_verts->push_back( localColor.y() );
            m_verts->push_back( localColor.z() );
            m_verts->push_back( globalColor.x() );
            m_verts->push_back( globalColor.y() );
            m_verts->push_back( globalColor.z() );
            m_verts->push_back( extra[k] );
            m_verts->push_back( 1.0 );

            m_verts->push_back( fib[k*3] );
            m_verts->push_back( fib[k*3+1] );
            m_verts->push_back( fib[k*3+2] );
            m_verts->push_back( localColor.x() );
            m_verts->push_back( localColor.y() );
            m_verts->push_back( localColor.z() );
            m_verts->push_back( globalColor.x() );
            m_verts->push_back( globalColor.y() );
            m_verts->push_back( globalColor.z() );
            m_verts->push_back( extra[k] );
            m_verts->push_back( -1.0 );
        }

        QVector3D localColor2( fib[numFloats-6] - fib[numFloats-3], fib[numFloats-5] - fib[numFloats-2], fib[numFloats-4] - fib[numFloats-1] );
        localColor.normalize();
        // push back the last vertex, done seperately because of nomal calculation
        m_verts->push_back( fib[numFloats-3] );
        m_verts->push_back( fib[numFloats-2] );
        m_verts->push_back( fib[numFloats-1] );
        m_verts->push_back( localColor2.x() );
        m_verts->push_back( localColor2.y() );
        m_verts->push_back( localColor2.z() );
        m_verts->push_back( globalColor.x() );
        m_verts->push_back( globalColor.y() );
        m_verts->push_back( globalColor.z() );
        m_verts->push_back( extra.last() );
        m_verts->push_back( 1.0 );

        m_verts->push_back( fib[numFloats-3] );
        m_verts->push_back( fib[numFloats-2] );
        m_verts->push_back( fib[numFloats-1] );
        m_verts->push_back( localColor2.x() );
        m_verts->push_back( localColor2.y() );
        m_verts->push_back( localColor2.z() );
        m_verts->push_back( globalColor.x() );
        m_verts->push_back( globalColor.y() );
        m_verts->push_back( globalColor.z() );
        m_verts->push_back( extra.last() );
        m_verts->push_back( -1.0 );
    }
}
