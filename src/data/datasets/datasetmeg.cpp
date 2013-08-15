/*
 * datasetmeg.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: schurade
 */

#include "datasetmeg.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"
#include "../../gui/gl/meshrenderer.h"

#include <QFile>
#include <QFileDialog>


DatasetMEG::DatasetMEG( QDir fn, Fn::DatasetType type ) :
    DatasetMesh( fn, type )
{
    m_dataMin = std::numeric_limits<float>().max();
    m_dataMax = std::numeric_limits<float>().min();
}

DatasetMEG::~DatasetMEG()
{
}

void DatasetMEG::addMesh( TriangleMesh2* tm, QString displayString )
{
    m_mesh.push_back( tm );
    m_displayList << displayString;
}

void DatasetMEG::addData( QVector<float> data )
{
    if ( data.size() == m_mesh[0]->numVerts() )
    {
        m_data.push_back( data );

        for ( int i = 0; i < data.size(); ++i )
        {
            m_dataMin = qMin( m_dataMin, data[i] );
            m_dataMax = qMax( m_dataMax, data[i] );
        }

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

void DatasetMEG::setProperties()
{
    m_properties["maingl"]->create( Fn::Property::D_SURFACE, m_displayList, 0, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_TEXTURE, 0, 0, m_data.size() - 1, "general" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_TEXTURE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectFrame() ) );
    m_properties["maingl2"]->create( Fn::Property::D_SURFACE, m_displayList, 0, "general" );
}

TriangleMesh2* DatasetMEG::getMesh( QString target )
{
    int n = properties( target )->get( Fn::Property::D_SURFACE ).toInt();
    return m_mesh[n];
}

void DatasetMEG::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
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

bool DatasetMEG::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    return false;
}

void DatasetMEG::selectFrame()
{
    int frame = properties( "maingl" )->get( Fn::Property::D_SELECTED_TEXTURE ).toInt();
    int n = properties( "maingl" )->get( Fn::Property::D_SURFACE ).toInt();
    TriangleMesh2* mesh = m_mesh[n];
    QVector<float>data = m_data[frame];
    for ( int i = 0; i < data.size(); ++i )
    {
        mesh->setVertexData( i, data[i] );
    }
}
