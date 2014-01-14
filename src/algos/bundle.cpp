/*
 * bundle.cpp
 *
 *  Created on: Nov 4, 2013
 *      Author: schurade
 */

#include "bundle.h"
#include "bundlethread.h"

#include "kdtree.h"

#include "../data/datasets/datasetfibers.h"

#include "../gui/gl/glfunctions.h"

#include <QThread>

Bundle::Bundle( DatasetFibers* ds ) :
    m_sourceDataset( ds ),
    m_threadsRunning( 0 ),
    m_currentLoop( 0 ),
    m_iterations( 10 ),
    m_radius( 10.0 ),
    m_smoothRange( 10.0 )
{
    m_fibs = m_sourceDataset->getFibs();
}

Bundle::~Bundle()
{
    m_fibs.clear();
    m_fibs.squeeze();
}

QVector< QVector< float > >Bundle::getFibs()
{
    return m_fibs;
}

void Bundle::start()
{
    startLoop();
}

void Bundle::startLoop()
{
    int numThreads = GLFunctions::idealThreadCount;
    ++m_currentLoop;
    qDebug() << "start loop:" << m_currentLoop;

    // build kdTree
    int numPoints = 0;
    m_kdVerts.clear();
    m_reverseIndexes.clear();
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        for( int k = 0; k < m_fibs[i].size() / 3; ++k )
        {
            m_kdVerts.push_back( m_fibs[i][k * 3    ] );
            m_kdVerts.push_back( m_fibs[i][k * 3 + 1] );
            m_kdVerts.push_back( m_fibs[i][k * 3 + 2] );
            m_reverseIndexes.push_back( i );
            ++numPoints;
        }
    }
    KdTree* kdTree = new KdTree( numPoints, m_kdVerts.data() );


    // create threads;
    for ( int i = 0; i < numThreads; ++i )
    {
        BundleThread* t = new BundleThread( i, &m_fibs, kdTree, &m_kdVerts, &m_reverseIndexes, numPoints );
        t->setRadius( m_radius );
        t->setIterations( m_iterations );
        m_threads.push_back( t );
        connect( t, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
        connect( t, SIGNAL( finished() ), this, SLOT( slotThreadFinished() ), Qt::QueuedConnection );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        ++m_threadsRunning;
        m_threads[i]->start();
    }


}

void Bundle::applyLoopResult()
{
    qDebug() << "apply";
    // apply movement to fibs
    for ( int i = 0; i < m_threads.size(); ++i )
    {
        int pc = 0;
        QVector< QVector3D > forces = m_threads[i]->getForces();
        for( int k = i; k < m_fibs.size(); k += m_threads.size() )
        {
            QVector<float>fib = m_fibs[k];
            float size = fib.size() / 3 - 1;

            for ( int l = 1; l < fib.size()/3 - 1; ++l )
            {
                QVector3D force = forces[pc++] / ( m_iterations * 0.50 );

                if ( l < ( ( size / 100 ) * m_smoothRange ) )
                {
                    float v = static_cast<float>( l ) / ( ( size / 100 ) * m_smoothRange );
                    fib[l*3] = fib[l*3] + force.x() * v;
                    fib[l*3+1] = fib[l*3+1] + force.y() * v;
                    fib[l*3+2] = fib[l*3+2] + force.z() * v;
                }
                else if ( l > ( ( size / 100 ) * ( 100. - m_smoothRange ) ) )
                {
                    float v = static_cast<float>( size - l ) / ( ( size / 100 ) * m_smoothRange );
                    fib[l*3] = fib[l*3] + force.x() * v;
                    fib[l*3+1] = fib[l*3+1] + force.y() * v;
                    fib[l*3+2] = fib[l*3+2] + force.z() * v;
                }
                else
                {
                    fib[l*3] = fib[l*3] + force.x();
                    fib[l*3+1] = fib[l*3+1] + force.y();
                    fib[l*3+2] = fib[l*3+2] + force.z();
                }
            }
            m_fibs.replace( k, fib );
        }
    }


    // delete threads
    for ( int i = 0; i < m_threads.size(); ++i )
    {
        delete m_threads[i];
    }
    m_threads.clear();
}

void Bundle::slotProgress()
{
    emit( progress() );
}

void Bundle::slotThreadFinished()
{
    emit( progress() );
    --m_threadsRunning;
    if ( !m_threadsRunning && m_currentLoop > m_iterations - 1 )
    {
        applyLoopResult();
        qDebug() << "all threads finished";

        qDebug() << "bundled " << m_fibs.size() << " fibers";
        qDebug() << "finished bundling";
        emit( finished() );
    }

    if ( !m_threadsRunning && m_currentLoop < m_iterations )
    {
        applyLoopResult();
        startLoop();
    }
}

void Bundle::setIterations( int value, int )
{
    m_iterations = value;
}

void Bundle::setRadius( float value, int )
{
    m_radius = value;
}

void Bundle::setSmoothRange( float value, int )
{
    m_smoothRange = value;
}
