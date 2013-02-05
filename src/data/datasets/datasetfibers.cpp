/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: schurade
 */
#include "datasetfibers.h"

#include "../../gui/gl/fiberrenderer.h"

DatasetFibers::DatasetFibers( QAbstractItemModel* roiModel, QString filename, QVector< QVector< float > > fibs, int numPoints, int numLines ) :
    Dataset( filename, Fn::DatasetType::FIBERS, roiModel ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, numLines );
    m_properties.set( Fn::Property::FIBER_COLORMODE, 0, 0, 1, true );
}

DatasetFibers::DatasetFibers( QAbstractItemModel* roiModel, QVector< QVector< float > > fibs ) :
    Dataset( QString("new fibers"), Fn::DatasetType::FIBERS, roiModel ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size();
    }
    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, fibs.size() );
    m_properties.set( Fn::Property::FIBER_COLORMODE, 0, 0, 1, true );
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
    m_renderer->setRenderParams( m_properties.get( Fn::Property::FIBER_COLORMODE ).toInt() );
    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
