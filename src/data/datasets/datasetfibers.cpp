/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: schurade
 */
#include "datasetfibers.h"

#include "../../gui/gl/fiberrenderer.h"

DatasetFibers::DatasetFibers( QString filename, QVector< QVector< float > > fibs, int numPoints, int numLines ) :
    Dataset( filename, FNDT_FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    m_properties.set( FNPROP_NUM_POINTS, numPoints );
    m_properties.set( FNPROP_NUM_LINES, numLines );
    m_properties.set( FNPROP_FIBER_COLORMODE, 0, 0, 1, true );
}

DatasetFibers::DatasetFibers( QVector< QVector< float > > fibs ) :
    Dataset( QString("new fibers"), FNDT_FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size();
    }
    m_properties.set( FNPROP_NUM_POINTS, numPoints );
    m_properties.set( FNPROP_NUM_LINES, fibs.size() );
    m_properties.set( FNPROP_FIBER_COLORMODE, 0, 0, 1, true );
}

DatasetFibers::~DatasetFibers()
{
    m_fibs.clear();
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

void DatasetFibers::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new FiberRenderer( m_fibs );
        m_renderer->setModel( model );
        m_renderer->init();
    }
    m_renderer->setRenderParams( m_properties.get( FNPROP_FIBER_COLORMODE ).toInt() );
    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
