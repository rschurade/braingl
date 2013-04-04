/*
 * datasetmesh.cpp
 *
 * Created on: Jul 19, 2012
 * @author Ralph Schurade
 */
#include "datasetmesh.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"
#include "../../gui/gl/meshrenderer.h"

DatasetMesh::DatasetMesh( TriangleMesh2* mesh, QString fileName ) :
    Dataset( fileName, Fn::DatasetType::MESH_BINARY ),
    m_mesh( mesh ),
    m_renderer( 0 )
{
    m_properties.set( Fn::Property::COLORMODE, 0, 0, 1 );
    m_properties.set( Fn::Property::COLORMAP, 1 );
    m_properties.set( Fn::Property::COLOR, QColor( 255, 255, 255 ), true );
}

DatasetMesh::~DatasetMesh()
{
}

void DatasetMesh::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }


    m_renderer->draw( pMatrix, mvMatrix, &m_properties );
}

QString DatasetMesh::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
