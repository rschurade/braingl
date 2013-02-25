/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: Ralph Schurade
 */
#include "datasetfibers.h"

#include "../../gui/gl/fiberrenderer.h"

DatasetFibers::DatasetFibers( QString filename, QVector< QVector< float > > fibs, int numPoints, int numLines ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, numLines );
    m_properties.set( Fn::Property::FIBER_COLORMODE, 0, 0, 3, true );
    m_properties.set( Fn::Property::FIBER_COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::Property::COLORMAP, 1 );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );

    m_extraData.reserve( fibs.size() );
    for ( int i = 0; i < fibs.size(); ++i )
    {
        m_extraData.push_back( QVector<float>( fibs[i].size() / 3 ) );
    }
}

DatasetFibers::DatasetFibers( QVector< QVector< float > > fibs ) :
    Dataset( QString("new fibers"), Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size() / 3;
    }
    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, fibs.size() );
    m_properties.set( Fn::Property::FIBER_COLORMODE, 0, 0, 3, true );
    m_properties.set( Fn::Property::FIBER_COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::Property::COLORMAP, 1 );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );

    m_extraData.reserve( fibs.size() );
    for ( int i = 0; i < fibs.size(); ++i )
    {
        m_extraData.push_back( QVector<float>( fibs[i].size() / 3 ) );
    }
}

DatasetFibers::DatasetFibers( QVector< QVector< float > > fibs, QVector< QVector< float > > extraData ) :
    Dataset( QString("new fibers"), Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_extraData( extraData ),
    m_renderer( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size() / 3;
    }
    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, fibs.size() );
    m_properties.set( Fn::Property::FIBER_COLORMODE, 0, 0, 4, true );
    m_properties.set( Fn::Property::FIBER_COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::Property::COLORMAP, 1, true );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f, true );
}


DatasetFibers::~DatasetFibers()
{
    m_fibs.clear();
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

QVector< QVector< float > > DatasetFibers::getSelectedFibs()
{
    if ( m_renderer == 0 )
    {
        return m_fibs;
    }
    else
    {
        QVector<bool>selected = m_renderer->getSelection();
        QVector<QVector<float> >out;
        for ( int i = 0; i < selected.size(); ++i )
        {
            if ( selected[i] )
            {
                out.push_back( m_fibs[i] );
            }
        }
        return out;
    }
}

void DatasetFibers::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QAbstractItemModel* dataModel )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new FiberRenderer( roiModel, m_fibs, m_extraData );
        m_renderer->setModel( globalModel );
        m_renderer->init();
        connect( m_properties.getProperty( Fn::Property::FIBER_COLOR ), SIGNAL( colorChanged( QColor ) ), m_renderer, SLOT( colorChanged( QColor ) ) );
    }
    m_renderer->setRenderParams( m_properties.get( Fn::Property::FIBER_COLORMODE ).toInt(),
                                 m_properties.get( Fn::Property::COLORMAP ).toInt(),
                                 m_properties.get( Fn::Property::SELECTED_MIN ).toFloat(),
                                 m_properties.get( Fn::Property::SELECTED_MAX ).toFloat(),
                                 m_properties.get( Fn::Property::LOWER_THRESHOLD ).toFloat(),
                                 m_properties.get( Fn::Property::UPPER_THRESHOLD ).toFloat() );
    m_renderer->draw( mvpMatrix, mvMatrixInverse, dataModel );
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
