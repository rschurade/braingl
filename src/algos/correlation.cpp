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
    int nroi = m_dataset->getMesh()->numVerts();
    int ntp = m_dataset->getNumDataPoints();

    m_exField = new float[ nroi ];
    m_ex2Field = new float[ nroi ];

    for ( int i = 0; i < nroi; ++i )
    {
        //calculate correlation btw. timeseries at i and j...
        double ex = 0;
        double ex2 = 0;
        float xk;
        for ( int k = 0; k < ntp; ++k )
        { //for all timepoints
            xk = m_dataset->getData( i, k ); //timepoint k at position i...
            ex += xk;
            ex2 += xk*xk;
        }
        m_exField[i] = ex;
        m_ex2Field[i] = ex2;
    }


    QAtomicInt currentId( 0 );
    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        CorrelationThread* t = new CorrelationThread( i, m_dataset, m_exField, m_ex2Field );
        m_threads.push_back( t );
        connect( t, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
        connect( t, SIGNAL( finished() ), this, SLOT( slotThreadFinished() ), Qt::QueuedConnection );
    }
    qDebug() << "start calculating correlation";
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
            QVector<std::vector<float> >* corr = m_threads[i]->getResult();

            for ( int k = 0; k < corr->size(); ++k )
            {
                for( unsigned int l = 0; l < corr->at( k ).size(); ++l )
                {
                    m_result[k * numThreads + i][(k * numThreads + i)+l] = corr->at( k ).at( l );
                    m_result[(k * numThreads + i)+l][k * numThreads + i] = corr->at( k ).at( l );
                }
            }
        }

        for ( int i = 0; i < m_threads.size(); ++i )
        {
            delete m_threads[i];
        }
        qDebug() << "finished calculating correlation";

//        16:24:44:737 [D] start calculating correlation
//        16:25:35:116 [D] finished calculating correlation

//        16:38:54:592 [D] start calculating correlation
//        16:39:43:759 [D] finished calculating correlation

        emit( finished() );
    }
}
