/*
 * correlation.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: schurade
 */

#include "correlation.h"
#include "correlationthread.h"

#include "../data/datasets/datasetmeshtimeseries.h"
#include "../data/mesh/trianglemesh2.h"

#include "../gui/gl/glfunctions.h"

Correlation::Correlation( DatasetMeshTimeSeries* ds ) :
    m_dataset( ds ),
    m_threadsRunning( 0 )
{
}

Correlation::~Correlation()
{
}

void Correlation::start()
{
    QAtomicInt currentId( 0 );
    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        CorrelationThread* t = new CorrelationThread( i, m_dataset );
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

float** Correlation::getResult()
{
    return m_result;
}

void Correlation::slotProgress()
{
    emit( progress() );
}

void Correlation::slotThreadFinished()
{
    --m_threadsRunning;
    if ( !m_threadsRunning )
    {
        qDebug() << "all threads finished";

        int m_n = m_dataset->getMesh()->numVerts();
        qDebug() << m_n;
        int numThreads = GLFunctions::idealThreadCount;

        m_result = new float*[m_n];
        for ( int i = 0; i < m_n; ++i )
        {
            m_result[i] = new float[m_n];
        }

        for ( int i = 0; i < m_threads.size(); ++i )
        {
            QVector<QVector<float> >* corr = m_threads[i]->getResult();

            for ( int k = 0; k < corr->size(); ++k )
            {
                for( int l = 0; l < corr->at( k).size(); ++l )
                {
                    m_result[k * numThreads + i][k+l] = corr->at( k ).at( l );
                    m_result[k+l][k * numThreads + i] = corr->at( k ).at( l );
                }
            }
        }

        for ( int i = 0; i < m_threads.size(); ++i )
        {
            delete m_threads[i];
        }
        emit( finished() );
    }
}
