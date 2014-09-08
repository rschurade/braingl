/*
 * sdthread.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#include "sdthread.h"

#include "../data/datasets/datasetdwi.h"

#include "../gui/gl/glfunctions.h"

SDThread::SDThread( DatasetDWI* ds, int id  ) :
    m_dataset( ds ),
    m_id( id )
{
    m_dwi = ds->getData();
    m_bvals = ds->getBvals();
    m_bvecs = ds->getBvecs();

    int nx = ds->properties( "maingl" ).get( Fn::Property::D_NX ).toInt();
    int ny = ds->properties( "maingl" ).get( Fn::Property::D_NY ).toInt();
    int nz = ds->properties( "maingl" ).get( Fn::Property::D_NZ ).toInt();
    m_blockSize = nx * ny *nz;
}

SDThread::~SDThread()
{
}

void SDThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;
    int progressCounter = 0;

    for ( int i = m_id; i < m_blockSize;  i += numThreads )
    {
        calcSD( i );

        ++progressCounter;
        if ( progressCounter == 100 )
        {
            emit( progress() );
            progressCounter = 0;
        }
    }
    emit( finished() );
}

ColumnVector SDThread::calcSD( int id )
{
    return ColumnVector( 20 );
}
