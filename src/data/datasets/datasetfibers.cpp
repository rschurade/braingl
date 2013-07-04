/*
 * datasetfibers.cpp
 *
 * Created on: Dec 12, 2012
 * @author Ralph Schurade
 */
#include "datasetfibers.h"

#include "fiberselector.h"
#include "../models.h"
#include "../../gui/gl/fiberrenderer.h"
#include "../../gui/gl/tuberenderer.h"


DatasetFibers::DatasetFibers( QDir filename, QVector< QVector< float > > fibs ) :
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

    m_properties["maingl"]->set( Fn::Property::D_NUM_POINTS, numPoints );
    m_properties["maingl"]->set( Fn::Property::D_NUM_LINES, fibs.size() );
    m_properties["maingl"]->set( Fn::Property::D_FIBER_RENDERMODE, {"lines", "tubes"}, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri" }, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "color" );
    m_properties["maingl"]->set( Fn::Property::D_ALPHA, 1.f, 0.01f, 1.f, "color" );
    m_properties["maingl"]->set( Fn::Property::D_FIBER_THICKNESS, 1.0f, 0.1f, 5.0f, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP, 1 );
    m_properties["maingl"]->set( Fn::Property::D_MIN, 0.0f );
    m_properties["maingl"]->set( Fn::Property::D_MAX, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_DX, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_DY, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_DZ, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NX, 800, 0, 1600, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NY, 1000, 0, 2000, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NZ, 800, 0, 1600, "special" );

    QVector<QVector<float> >data0;
    QVector<float>min0;
    QVector<float>max0;
    data0.reserve( fibs.size() );
    for ( int i = 0; i < fibs.size(); ++i )
    {
        data0.push_back( QVector<float>( fibs[i].size() / 3 ) );
    }
    m_data.push_back( data0 );
    m_dataNames.push_back( "no data" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_COLOR ), SIGNAL( colorChanged( QColor ) ), this, SLOT( colorChanged() ) );

    m_properties["maingl2"]->set( Fn::Property::D_NUM_POINTS, numPoints );
    m_properties["maingl2"]->set( Fn::Property::D_NUM_LINES, fibs.size() );
    m_properties["maingl2"]->set( Fn::Property::D_FIBER_RENDERMODE, {"lines", "tubes"}, 0, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri" }, 0, "general" );
    //m_properties["maingl2"]->set( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), true );
    m_properties["maingl2"]->set( Fn::Property::D_ALPHA, 1.f, 0.f, 1.f, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_FIBER_THICKNESS, 1.0f, 0.1f, 5.0f, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP, 1 );
    m_properties["maingl2"]->set( Fn::Property::D_MIN, 0.0f );
    m_properties["maingl2"]->set( Fn::Property::D_MAX, 1.0f );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties["maingl2"]->set( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties["maingl2"]->set( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    m_properties["maingl2"]->set( Fn::Property::D_DX, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl2"]->set( Fn::Property::D_DY, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl2"]->set( Fn::Property::D_DZ, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl2"]->set( Fn::Property::D_NX, 800, 0, 1600, "special" );
    m_properties["maingl2"]->set( Fn::Property::D_NY, 1000, 0, 2000, "special" );
    m_properties["maingl2"]->set( Fn::Property::D_NZ, 800, 0, 1600, "special" );
}

DatasetFibers::DatasetFibers( QDir filename,
                                 QVector< QVector< float > > fibs,
                                 QVector< QVector< QVector< float > > >data,
                                 QVector<QString>dataNames,
                                 QVector< float > mins,
                                 QVector<float> maxes ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_data( data ),
    m_dataNames( dataNames ),
    m_dataMins( mins ),
    m_dataMaxes( maxes ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 )
{
    int numPoints = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size() / 3;
    }

    m_properties["maingl"]->set( Fn::Property::D_NUM_POINTS, numPoints );
    m_properties["maingl"]->set( Fn::Property::D_NUM_LINES, fibs.size() );
    m_properties["maingl"]->set( Fn::Property::D_FIBER_RENDERMODE, {"lines", "tubes"}, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri", "data" }, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_DATAMODE, dataNames, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties["maingl"]->set( Fn::Property::D_ALPHA, 1.f, 0.f, 1.f, "general" );
    m_properties["maingl"]->set( Fn::Property::D_FIBER_THICKNESS, 1.0f, 0.1f, 5.0f, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP, 1, "color" );
    m_properties["maingl"]->set( Fn::Property::D_MIN, 0.0f );
    m_properties["maingl"]->set( Fn::Property::D_MAX, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f, "color" );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f, "color" );
    m_properties["maingl"]->set( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::D_DX, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_DY, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_DZ, 100.0f, 0.0f, 100.0f, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NX, 800, 0, 1600, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NY, 1000, 0, 2000, "special" );
    m_properties["maingl"]->set( Fn::Property::D_NZ, 800, 0, 1600, "special" );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( float ) ) );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_COLOR ), SIGNAL( colorChanged( QColor ) ), this, SLOT( colorChanged() ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_DATAMODE ), SIGNAL( valueChanged( int ) ), this, SLOT( dataModeChanged() ) );
}

DatasetFibers::~DatasetFibers()
{
    m_properties["maingl"]->set( Fn::Property::D_ACTIVE, false );
    m_properties["maingl2"]->set( Fn::Property::D_ACTIVE, false );
    m_fibs.clear();
    m_fibs.squeeze();
    if ( m_renderer != 0 )
    {
        delete m_renderer;
        m_renderer = 0;
    }
    if ( m_tubeRenderer != 0 )
    {
        delete m_tubeRenderer;
        m_tubeRenderer = 0;
    }
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

QVector< QVector< QVector< float > > > DatasetFibers::getData()
{
    return m_data;
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

QVector< QVector< QVector< float > > > DatasetFibers::getSelectedData()
{
    if ( m_renderer == 0 )
    {
        return m_data;
    }
    else
    {
        QVector<bool>*selected = m_selector->getSelection();
        QVector< QVector<QVector<float> > >out;

        for ( int k = 0; k < m_data.size(); ++k )
        {
            QVector<QVector<float> > data = m_data[k];
            QVector<QVector<float> > dataOut;
            for ( int i = 0; i < selected->size(); ++i )
            {
                if ( selected->at( i ) )
                {
                    dataOut.push_back( data[i] );
                }
            }
            out.push_back( dataOut );
        }
        return out;
    }
}

QVector<float> DatasetFibers::getDataMins()
{
    return m_dataMins;
}

QVector<float> DatasetFibers::getDataMaxes()
{
    return m_dataMaxes;
}

QVector<QString> DatasetFibers::getDataNames()
{
    return m_dataNames;
}

void DatasetFibers::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_selector == 0 )
    {
        m_selector = new FiberSelector();
        m_selector->init( m_fibs );
        connect( m_selector, SIGNAL( changed() ), Models::d(), SLOT( submit() ) );
    }

    if ( properties( target )->get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 0 )
    {
        if ( m_renderer == 0 )
        {
            m_renderer = new FiberRenderer( m_selector, m_fibs, m_data[properties( target )->get( Fn::Property::D_DATAMODE).toInt()] );
            m_renderer->setModel( Models::g() );
            m_renderer->init();
            m_renderer->colorChanged( properties( target )->get( Fn::Property::D_COLOR ).value<QColor>() );
            connect( properties( target )->getProperty( Fn::Property::D_COLOR ), SIGNAL( colorChanged( QColor ) ), m_renderer, SLOT( colorChanged( QColor ) ) );
        }

        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
    else if ( properties( target )->get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 1 )
    {
        if ( m_tubeRenderer == 0 )
        {
            m_tubeRenderer = new TubeRenderer( m_selector, m_fibs, m_data[properties( target )->get( Fn::Property::D_DATAMODE).toInt()] );
            m_tubeRenderer->setModel( Models::g() );
            m_tubeRenderer->init();
            m_tubeRenderer->colorChanged( properties( target )->get( Fn::Property::D_COLOR ).value<QColor>() );
            connect( properties( target )->getProperty( Fn::Property::D_COLOR ), SIGNAL( colorChanged( QColor ) ), m_tubeRenderer, SLOT( colorChanged( QColor ) ) );
        }

        m_tubeRenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

void DatasetFibers::colorChanged()
{
    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, 2 );
}

void DatasetFibers::dataModeChanged()
{
    if ( m_renderer != 0 )
    {
        delete m_renderer;
        m_renderer = 0;
    }
    if ( m_tubeRenderer != 0 )
    {
        delete m_tubeRenderer;
        m_tubeRenderer = 0;
    }
    float min = m_dataMins[ m_properties["maingl"]->get( Fn::Property::D_DATAMODE).toInt()];
    float max = m_dataMaxes[ m_properties["maingl"]->get( Fn::Property::D_DATAMODE).toInt()];

    m_properties["maingl"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl"]->set( Fn::Property::D_MAX, max );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ) )->setMin( min );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ) )->setMax( max );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ) )->setMin( min );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ) )->setMax( max );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ) )->setValue( min );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ) )->setValue( max );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ) )->setValue( min );
    static_cast<PropertyFloat*> ( m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ) )->setValue( max );
    Models::d()->submit();
}
