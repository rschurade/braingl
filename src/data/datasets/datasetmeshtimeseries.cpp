/*
 * datasetmeg.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: schurade
 */

#include "datasetmeshtimeseries.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"
#include "../../gui/gl/meshrenderer.h"
#include "../../gui/gl/colormapfunctions.h"

#include "../../algos/colormapbase.h"

#include <QFile>
#include <QFileDialog>


DatasetMeshTimeSeries::DatasetMeshTimeSeries( QDir fn, Fn::DatasetType type ) :
    DatasetMesh( fn, type )
{
    m_dataMin = std::numeric_limits<float>().max();
    m_dataMax = std::numeric_limits<float>().min();
    finalizeProperties();
}

DatasetMeshTimeSeries::~DatasetMeshTimeSeries()
{
}

void DatasetMeshTimeSeries::addMesh( TriangleMesh2* tm, QString displayString )
{
    m_mesh.push_back( tm );
    m_displayList.push_back( displayString );

    m_properties["maingl"]->set( Fn::Property::D_MESH_NUM_VERTEX, m_mesh[0]->numVerts() );
    m_properties["maingl"]->set( Fn::Property::D_MESH_NUM_TRIANGLES, m_mesh[0]->numTris() );
    m_properties["maingl"]->set( Fn::Property::D_START_INDEX, 0 );
    m_properties["maingl"]->set( Fn::Property::D_END_INDEX, m_mesh[0]->numTris() );
}

void DatasetMeshTimeSeries::addData( std::vector<float> data )
{
    if ( data.size() == m_mesh[0]->numVerts() )
    {
        m_data.push_back( data );

        for ( unsigned int i = 0; i < data.size(); ++i )
        {
            m_dataMin = qMin( m_dataMin, data[i] );
            m_dataMax = qMax( m_dataMax, data[i] );
        }
        qDebug() << "min/max" << m_dataMin << m_dataMax;

        m_properties["maingl"]->getProperty( Fn::Property::D_MIN )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_MIN )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_MIN )->setValue( m_dataMin );

        m_properties["maingl"]->getProperty( Fn::Property::D_MAX )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_MAX )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_MAX )->setValue( m_dataMax );

        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setValue( m_dataMin );

        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setValue( m_dataMax );

        m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD )->setValue( m_dataMin );

        m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD )->setMin( m_dataMin );
        m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD )->setMax( m_dataMax );
        m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD )->setValue( m_dataMax );
    }
}

void DatasetMeshTimeSeries::setProperties()
{
    m_properties["maingl"]->createList( Fn::Property::D_SURFACE, m_displayList, 0, "general" );
    m_properties["maingl"]->createInt( Fn::Property::D_SELECTED_TEXTURE, 0, 0, m_data.size() - 1, "general" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_TEXTURE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectFrame() ) );
    connect ( m_properties["maingl"], SIGNAL( signalSetProp( int ) ), this, SLOT( slotPropSet( int ) ) );
    m_properties["maingl2"]->createList( Fn::Property::D_SURFACE, m_displayList, 0, "general" );

//    m_properties["maingl"]->createButton( Fn::Property::D_COPY_COLORS, "general" );
//    connect( m_properties["maingl"]->getProperty( Fn::Property::D_COPY_COLORS ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( slotCopyColors() ) );
}

void DatasetMeshTimeSeries::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( getMesh( target ) );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }
    m_renderer->setMesh( getMesh( target ) );
    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

bool DatasetMeshTimeSeries::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    return false;
}

void DatasetMeshTimeSeries::selectFrame()
{
    int frame = properties( "maingl" )->get( Fn::Property::D_SELECTED_TEXTURE ).toInt();
    int n = properties( "maingl" )->get( Fn::Property::D_SURFACE ).toInt();
    TriangleMesh2* mesh = m_mesh[n];
    std::vector<float>data = m_data[frame];
    for ( unsigned int i = 0; i < data.size(); ++i )
    {
        mesh->setVertexData( i, data[i] );
    }
}

void DatasetMeshTimeSeries::slotPropSet( int id )
{
    if ( id == (int)Fn::Property::D_SELECTED_TEXTURE )
    {
        selectFrame();
    }
}

void DatasetMeshTimeSeries::slotCopyColors()
{
    int frame = properties( "maingl" )->get( Fn::Property::D_SELECTED_TEXTURE ).toInt();
    int n = properties( "maingl" )->get( Fn::Property::D_SURFACE ).toInt();
    TriangleMesh2* mesh = m_mesh[n];
    std::vector<float>data = m_data[frame];

    QColor color;

    float selectedMin = properties( "maingl" )->get( Fn::Property::D_SELECTED_MIN ).toFloat();
    float selectedMax = properties( "maingl" )->get( Fn::Property::D_SELECTED_MAX ).toFloat();

    for ( unsigned int i = 0; i < data.size(); ++i )
    {
        ColormapBase cmap = ColormapFunctions::getColormap( properties( "maingl" )->get( Fn::Property::D_COLORMAP ).toInt() );

        float value = ( data[i] - selectedMin ) / ( selectedMax - selectedMin );
        color = cmap.getColor( qMax( 0.0f, qMin( 1.0f, value ) ) );

        mesh->setVertexColor( i, color );
    }
}

int DatasetMeshTimeSeries::getNumDataPoints()
{
    return m_data.size();
}

float DatasetMeshTimeSeries::getData( int id, int dataPoint )
{
    return m_data.at(dataPoint).at( id );
}
