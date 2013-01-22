/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: schurade
 */
#include "../datastore.h"
#include "../../gui/gl/fiberrenderer.h"

#include "datasetfibers.h"

DatasetFibers::DatasetFibers( QVector< QVector< float > > fibs, int numPoints, int numLines ) :
    Dataset( QString("fibers"), FNDT_FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    m_properties.set( FNPROP_NUM_POINTS, numPoints );
    m_properties.set( FNPROP_NUM_LINES, numLines );
}

DatasetFibers::~DatasetFibers()
{
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

void DatasetFibers::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new FiberRenderer( m_fibs );
        m_renderer->setModel( dataStore );
        m_renderer->init();
    }

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}


