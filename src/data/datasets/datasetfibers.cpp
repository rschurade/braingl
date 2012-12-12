/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: schurade
 */

#include "datasetfibers.h"

DatasetFibers::DatasetFibers( QVector< QVector< float > > fibs, int numPoints, int numLines ) :
    Dataset( QString("fibers"), FNDT_FIBERS ),
    m_fibs( fibs )
{
    setProperty( "numPoints", numPoints );
    setProperty( "numLines", numLines );
}

DatasetFibers::~DatasetFibers()
{
    // TODO Auto-generated destructor stub
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

void DatasetFibers::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore )
{
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}


