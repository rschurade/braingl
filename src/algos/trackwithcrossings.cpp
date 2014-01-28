/*
 * trackwithcrossings.cpp
 *
 * Created on: Jan 28, 2013
 * @author Ralph Schurade
 */
#include "trackwithcrossings.h"
#include "twcthread.h"

#include "fmath.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasettensor.h"

#include "../gui/gl/glfunctions.h"

#include <QThread>

#include "time.h"
#include "math.h"

TrackWithCrossings::TrackWithCrossings() :
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 ),

    m_minLength( 10 ),
    m_stepSize( 1.0 ),
    m_diag( 1.0 ),
    maxStepsInVoxel( 3 ),
    m_smoothness( 0.0 ),

    m_threadsRunning( 0 ),
    m_thinOut( false ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
}

void TrackWithCrossings::setDatasets( DatasetScalar* mask, DatasetTensor* ds1, DatasetTensor* ds2, DatasetTensor* ds3 )
{
    m_mask = mask;
    m_ds1 = ds1;
    m_ds2 = ds2;
    m_ds3 = ds3;

    m_nx = ds1->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
    m_ny = ds1->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
    m_nz = ds1->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();
    m_dx = ds1->properties( "maingl" )->get( Fn::Property::D_DX ).toFloat();
    m_dy = ds1->properties( "maingl" )->get( Fn::Property::D_DY ).toFloat();
    m_dz = ds1->properties( "maingl" )->get( Fn::Property::D_DZ ).toFloat();

    m_diag = sqrt( m_dx * m_dx + m_dy * m_dy + m_dz * m_dz );
    maxStepsInVoxel = ( (int) ( m_diag / m_stepSize ) + 1 ) * 2;

    blockSize = m_nx * m_ny * m_nz;
}

TrackWithCrossings::~TrackWithCrossings()
{
}

std::vector<Fib> TrackWithCrossings::getFibs()
{
    return m_fibs;
}

int TrackWithCrossings::getNumPoints()
{
    return m_numPoints;
}

int TrackWithCrossings::getNumLines()
{
    return m_numLines;
}

void TrackWithCrossings::startTracking()
{
    srand( time( 0 ) );

    qDebug() << m_dx << m_dy << m_dz;
    qDebug() << "diag: " << m_diag << " msiv: " << maxStepsInVoxel;
    qDebug() << "smoothness: " << m_smoothness;
    qDebug() << "min length: " << m_minLength / m_stepSize;

    qDebug() << "start tracking";
    trackWholeBrain();
}

void TrackWithCrossings::trackWholeBrain()
{
    std::vector<float>* mask = m_mask->getData();
    std::vector<Matrix>* tensors1 = m_ds1->getData();
    std::vector<Matrix>* logtensors1 = m_ds1->getLogData();
    std::vector<Matrix>* tensors2 = m_ds2->getData();
    std::vector<Matrix>* logtensors2 = m_ds2->getLogData();
    std::vector<Matrix>* tensors3 = m_ds3->getData();
    std::vector<Matrix>* logtensors3 = m_ds3->getLogData();

    std::vector<QVector3D>* evec1 = new std::vector<QVector3D>();
    std::vector<QVector3D>* evec2 = new std::vector<QVector3D>();
    std::vector<QVector3D>* evec3 = new std::vector<QVector3D>();
    qDebug() << "create evecs";
    FMath::evec1( *tensors1, *evec1 );
    FMath::evec1( *tensors2, *evec2 );
    FMath::evec1( *tensors3, *evec3 );
    qDebug() << "done evecs";

    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        TWCThread* t = new TWCThread(  i, mask, logtensors1, logtensors2, logtensors3, evec1, evec2, evec3, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz );
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

void TrackWithCrossings::slotProgress()
{
    emit( progress() );
}

void TrackWithCrossings::slotThreadFinished()
{
    --m_threadsRunning;
    if ( !m_threadsRunning )
    {
        qDebug() << "all threads finished";
        // combine fibs from all threads
        for ( unsigned int i = 0; i < m_threads.size(); ++i )
        {
            m_fibs.insert( m_fibs.end(), m_threads[i]->getFibs()->begin(), m_threads[i]->getFibs()->end() );
        }

        for ( unsigned int i = 0; i < m_threads.size(); ++i )
        {
            delete m_threads[i];
        }
        qDebug() << "tracked " << m_fibs.size() << " fibers";
        qDebug() << "finished tracking";
        emit( finished() );
    }
}

void TrackWithCrossings::setMinLength( int value, int )
{
    m_minLength = value;
}

void TrackWithCrossings::setStepSize( float value, int )
{
    m_stepSize = value;
}

void TrackWithCrossings::setSmoothness( float value, int )
{
    m_smoothness = value;
}

