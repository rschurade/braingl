/*
 * track.cpp
 *
 * Created on: Dec 11, 2012
 * @author Ralph Schurade
 */
#include "track.h"
#include "trackthread.h"

#include "fmath.h"
#include "time.h"

#include "../data/datasets/datasettensor.h"

#include "../gui/gl/glfunctions.h"

#include <QThread>

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

    m_threadsRunning( 0 ),
    m_thinOut( false ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
    m_nx = ds->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
    m_ny = ds->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
    m_nz = ds->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();
    m_dx = ds->properties( "maingl" )->get( Fn::Property::D_DX ).toFloat();
    m_dy = ds->properties( "maingl" )->get( Fn::Property::D_DY ).toFloat();
    m_dz = ds->properties( "maingl" )->get( Fn::Property::D_DZ ).toFloat();

    m_diag = sqrt( m_dx * m_dx + m_dy * m_dy + m_dz * m_dz );
    maxStepsInVoxel = ( (int) ( m_diag / m_stepSize ) + 1 ) * 2;

    blockSize = m_nx * m_ny * m_nz;
}

Track::~Track()
{
}

QVector< std::vector<float> > Track::getFibs()
{
    return fibs;
}

QVector< std::vector<float> > Track::getExtras()
{
    return extras;
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
    m_tensors = m_dataset->getData();
    m_logTensors = m_dataset->getLogData();
    FMath::evec1( *m_tensors, m_evec1 );
    FMath::fa( *m_tensors, m_fa );

    srand( time( 0 ) );

    qDebug() << m_dx << m_dy << m_dz;
    qDebug() << "diag: " << m_diag << " msiv: " << maxStepsInVoxel;
    qDebug() << "smoothness: " << m_smoothness;
    qDebug() << "min length: " << m_minLength / m_stepSize;

    qDebug() << "start tracking";
    trackWholeBrain();
}

void Track::trackWholeBrain()
{
    QAtomicInt currentId( 0 );
    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        TrackThread* t = new TrackThread( m_tensors, m_logTensors, &m_fa, &m_evec1, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, i, m_minLength, m_minFA, m_minStartFA, m_stepSize, m_smoothness );
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

void Track::slotProgress()
{
    emit( progress() );
}

void Track::slotThreadFinished()
{
    --m_threadsRunning;
    if ( !m_threadsRunning )
    {
        qDebug() << "all threads finished";
        // combine fibs from all threads
        for ( int i = 0; i < m_threads.size(); ++i )
        {
            fibs += m_threads[i]->getFibs();
            extras += m_threads[i]->getExtras();
        }

        for ( int i = 0; i < m_threads.size(); ++i )
        {
            delete m_threads[i];
        }
        qDebug() << "tracked " << fibs.size() << " fibers";
        qDebug() << "finished tracking";
        emit( finished() );
    }
}

void Track::setMinLength( int value, int )
{
    m_minLength = value;
}

void Track::setMinFA( float value, int )
{
    m_minFA = value;
}

void Track::setMinStartFA( float value, int )
{
    m_minStartFA = value;
}

void Track::setStepSize( float value, int )
{
    m_stepSize = value;
}

void Track::setSmoothness( float value, int )
{
    m_smoothness = value;
}

