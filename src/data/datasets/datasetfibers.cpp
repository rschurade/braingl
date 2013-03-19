/*
 * datasetfibers.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: Ralph Schurade
 */
#include "datasetfibers.h"

#include "fiberselector.h"
#include "../models.h"
#include "../../gui/gl/fiberrenderer.h"
#include "../../gui/gl/tuberenderer.h"


DatasetFibers::DatasetFibers( QString filename, QVector< QVector< float > > fibs ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size() / 3;
    }

    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, fibs.size() );
    m_properties.set( Fn::Property::FIBER_RENDERMODE, 0, 0, 1, true );
    m_properties.set( Fn::Property::COLORMODE, 0, 0, 3, true );
    m_properties.set( Fn::Property::FIBER_COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::Property::FIBER_TUBE_THICKNESS, 0.01f, 0.001f, 0.2f, true );
    m_properties.set( Fn::Property::FIBER_LINE_THICKNESS, 1.0f, 1.0, 5.0f, true );
    m_properties.set( Fn::Property::COLORMAP, 1 );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::DX, 100.0f, 0.0f, 100.0f, true );
    m_properties.set( Fn::Property::DY, 100.0f, 0.0f, 100.0f, true );
    m_properties.set( Fn::Property::DZ, 100.0f, 0.0f, 100.0f, true );
    m_properties.set( Fn::Property::NX, 800, 0, 1600, true );
    m_properties.set( Fn::Property::NY, 1000, 0, 2000, true );
    m_properties.set( Fn::Property::NZ, 800, 0, 1600, true );

    m_extraData.reserve( fibs.size() );
    for ( int i = 0; i < fibs.size(); ++i )
    {
        m_extraData.push_back( QVector<float>( fibs[i].size() / 3 ) );
    }
}

DatasetFibers::DatasetFibers( QString filename, QVector< QVector< float > > fibs, QVector< QVector< float > > extras ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_extraData( extras ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size() / 3;
    }

    m_properties.set( Fn::Property::NUM_POINTS, numPoints );
    m_properties.set( Fn::Property::NUM_LINES, fibs.size() );
    m_properties.set( Fn::Property::FIBER_RENDERMODE, 0, 0, 1, true );
    m_properties.set( Fn::Property::COLORMODE, 0, 0, 4, true );
    m_properties.set( Fn::Property::FIBER_COLOR, QColor( 255, 0, 0 ), true );
    m_properties.set( Fn::Property::FIBER_TUBE_THICKNESS, 0.01f, 0.001f, 0.2f, true );
    m_properties.set( Fn::Property::COLORMAP, 1, true );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );

    connect( m_properties.getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties.getProperty( Fn::Property::LOWER_THRESHOLD ), SLOT( setMax( float ) ) );
    connect( m_properties.getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties.getProperty( Fn::Property::UPPER_THRESHOLD ), SLOT( setMin( float ) ) );

    connect( m_properties.getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties.getProperty( Fn::Property::SELECTED_MAX ), SLOT( setMin( float ) ) );
    connect( m_properties.getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties.getProperty( Fn::Property::SELECTED_MIN ), SLOT( setMax( float ) ) );
}

DatasetFibers::~DatasetFibers()
{
    m_fibs.clear();
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

QVector< QVector< float > > DatasetFibers::getExtra()
{
    return m_extraData;
}

QVector< QVector< float > > DatasetFibers::getSelectedFibs()
{
    if ( m_renderer == 0 )
    {
        return m_fibs;
    }
    else
    {
        QVector<bool>*selected = m_selector->getSelection();
        QVector<QVector<float> >out;
        for ( int i = 0; i < selected->size(); ++i )
        {
            if ( selected->at( i ) )
            {
                out.push_back( m_fibs[i] );
            }
        }
        return out;
    }
}

void DatasetFibers::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
    if ( m_selector == 0 )
    {
        m_selector = new FiberSelector();
        m_selector->init( m_fibs );
        connect( m_selector, SIGNAL( changed() ), Models::d(), SLOT( submit() ) );
    }

    if ( m_properties.get( Fn::Property::FIBER_RENDERMODE).toInt() == 0 )
    {
        if ( m_renderer == 0 )
        {
            m_renderer = new FiberRenderer( m_selector, m_fibs, m_extraData );
            m_renderer->setModel( Models::g() );
            m_renderer->init();
            connect( m_properties.getProperty( Fn::Property::FIBER_COLOR ), SIGNAL( colorChanged( QColor ) ), m_renderer, SLOT( colorChanged( QColor ) ) );
        }

        m_renderer->draw( pMatrix, mvMatrix, &m_properties );
    }
    else if ( m_properties.get( Fn::Property::FIBER_RENDERMODE).toInt() == 1 )
    {
        if ( m_tubeRenderer == 0 )
        {
            m_tubeRenderer = new TubeRenderer( m_selector, m_fibs, m_extraData );
            m_tubeRenderer->setModel( Models::g() );
            m_tubeRenderer->init();
            connect( m_properties.getProperty( Fn::Property::FIBER_COLOR ), SIGNAL( colorChanged( QColor ) ), m_tubeRenderer, SLOT( colorChanged( QColor ) ) );
        }

        m_tubeRenderer->draw( pMatrix, mvMatrix, &m_properties );
    }
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
