/*
 * sd.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#include "sd.h"
#include "sdthread.h"

#include "../data/datasets/datasetdwi.h"

#include "../gui/gl/glfunctions.h"

SD::SD( DatasetDWI* ds ) :
    m_dataset( ds ),
    m_threadsRunning( 0 )
{
}

SD::~SD()
{
}

void SD::start()
{
    QAtomicInt currentId( 0 );
    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        SDThread* t = new SDThread( m_dataset, i );
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

void SD::slotProgress()
{
    emit( progress() );
}

void SD::slotThreadFinished()
{
    --m_threadsRunning;
    if ( !m_threadsRunning )
    {
        qDebug() << "all threads finished";
        // combine fibs from all threads
        for ( int i = 0; i < m_threads.size(); ++i )
        {
        }

        for ( int i = 0; i < m_threads.size(); ++i )
        {
            delete m_threads[i];
        }
        qDebug() << "finished sd";
        emit( finished() );
    }
}
