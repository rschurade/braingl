/*
 * correlationthread.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: schurade
 */

#include "correlationthread.h"

#include "../data/datasets/datasetmeshtimeseries.h"

#include "../data/mesh/trianglemesh2.h"

#include "../gui/gl/glfunctions.h"

CorrelationThread::CorrelationThread( int id, DatasetMeshTimeSeries* ds, float* exField, float* ex2Field ) :
    m_id( id ),
    m_dataset( ds ),
    m_exField( exField ),
    m_ex2Field( ex2Field )
{
}

CorrelationThread::~CorrelationThread()
{

}

std::vector< std::vector<float> >* CorrelationThread::getResult()
{
    return &m_result;
}

void CorrelationThread::run()
{
    int nroi = m_dataset->getMesh()->numVerts();
    int ntp = m_dataset->getNumDataPoints();

    int numThreads = GLFunctions::idealThreadCount;

    int progressCounter = 0;

    for ( int i = m_id; i < nroi; i += numThreads )
    {
        std::vector<float> corVec;
        for ( int j = i; j < nroi; ++j )
        {
            //calculate correlation btw. timeseries at i and j...
            double ex = m_exField[i];
            double ey = m_exField[j];
            double exy = 0;
            double ex2 = m_ex2Field[i];
            double ey2 = m_ex2Field[j];
            for ( int k = 0; k < ntp; ++k )
            { //for all timepoints
                float xk, yk;

                //this needs to get connected to whatever data representation of the timeseries data is used:
                xk = m_dataset->getData( i, k ); //timepoint k at position i...
                yk = m_dataset->getData( j, k );
                exy += xk*yk;
            }

            float corr = (ntp*exy - ex*ey) / sqrt((ntp*ex2 - ex*ex) * (ntp*ey2 - ey*ey));

            //the correlation matrix is symmetric:
            //saving that value to both position i,j and j,i at once takes ~ half the time
            //we could also think about a more compact representation of the matrix...
            corVec.push_back( corr );
        }
        m_result.push_back( corVec );
        ++progressCounter;
        if ( progressCounter == 100 )
        {
            emit( progress() );
            progressCounter = 0;
        }
    }
    emit( finished() );
}
