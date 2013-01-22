/*
 * track.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: schurade
 */
#include <QtCore/QThread>

#include "../data/datasets/datasettensor.h"

#include "fmath.h"
#include "time.h"

#include "trackthread.h"
#include "track.h"

Track::Track( DatasetTensor* ds ) :
    m_dataset( ds ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 ),

    m_minLength( 10 ),
    m_minFA( 0.2 ),
    m_minStartFA( 0.2 ),
    m_stepSize( 1.0 ),
    m_diag( 1.0 ),
    maxStepsInVoxel( 3 ),
    m_smoothness( 0.0 ),

    m_threads( 4 ),
    m_count ( 0 ),
    m_thinOut( false ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
    m_tensors = ds->getData();
    m_logTensors = ds->getLogData();

    m_nx = ds->properties()->get( FNPROP_NX ).toInt();
    m_ny = ds->properties()->get( FNPROP_NY ).toInt();
    m_nz = ds->properties()->get( FNPROP_NZ ).toInt();
    m_dx = ds->properties()->get( FNPROP_DX ).toFloat();
    m_dy = ds->properties()->get( FNPROP_DY ).toFloat();
    m_dz = ds->properties()->get( FNPROP_DZ ).toFloat();

    qDebug() << m_dx << m_dy << m_dz;
    m_diag = sqrt( m_dx * m_dx + m_dy * m_dy + m_dz * m_dz );
    maxStepsInVoxel = ( (int) ( m_diag / m_stepSize ) + 1 ) * 2;

    blockSize = m_nx * m_ny * m_nz;

    FMath::evec1( m_tensors, m_evec1 );
    FMath::fa( m_tensors, m_fa );
}

Track::~Track()
{
}

QVector< QVector< float > > Track::getFibs()
{
    return fibs;
}

int Track::getNumPoints()
{
    return m_numPoints;
}

int Track::getNumLines()
{
    return m_numLines;
}

void Track::startTracking()
{
    srand( time( 0 ) );

    qDebug() << "diag: " << m_diag << " msiv: " << maxStepsInVoxel;
    qDebug() << "smoothness: " << m_smoothness;
    qDebug() << "min length: " << m_minLength / m_stepSize;

    qDebug() << "start tracking";
    trackWholeBrain();
    qDebug() << "finished tracking";
}

void Track::trackWholeBrain()
{
    QAtomicInt currentId( 0 );
    int numThreads = QThread::idealThreadCount();

    QVector<TrackThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new TrackThread( m_tensors, m_logTensors, &m_fa, &m_evec1, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, i ) );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->wait();
    }
    // combine fibs from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        fibs += threads[i]->getFibs();
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }
    qDebug() << "tracked " << fibs.size() << " fibers";
}
