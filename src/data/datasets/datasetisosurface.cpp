/*
 * datasetisosurface.cpp
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */
#include "datasetisosurface.h"

#include "datasetmesh.h"
#include "datasetscalar.h"
#include "isosurfacethread.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/glfunctions.h"
#include "../../gui/gl/meshrenderer.h"

#include <QString>

DatasetIsosurface::DatasetIsosurface( DatasetScalar* ds ) :
        DatasetMesh( QString( "isosurface" ), Fn::DatasetType::MESH_ISOSURFACE ),
        m_oldIsoValue( -1 )
{
    m_scalarField = *(ds->getData() );

    m_properties["maingl"]->set( Fn::Property::NX, ds->properties( "maingl" )->get( Fn::Property::NX ).toInt() );
    m_properties["maingl"]->set( Fn::Property::NY, ds->properties( "maingl" )->get( Fn::Property::NY ).toInt() );
    m_properties["maingl"]->set( Fn::Property::NZ, ds->properties( "maingl" )->get( Fn::Property::NZ ).toInt() );
    m_properties["maingl"]->set( Fn::Property::DX, ds->properties( "maingl" )->get( Fn::Property::DX ).toFloat() );
    m_properties["maingl"]->set( Fn::Property::DY, ds->properties( "maingl" )->get( Fn::Property::DY ).toFloat() );
    m_properties["maingl"]->set( Fn::Property::DZ, ds->properties( "maingl" )->get( Fn::Property::DZ ).toFloat() );

    m_properties["maingl"]->set( Fn::Property::DIM, 0 );
    m_properties["maingl"]->set( Fn::Property::CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties["maingl"]->set( Fn::Property::TYPE, (int)Fn::DatasetType::MESH_ISOSURFACE );
    m_properties["maingl"]->set( Fn::Property::NAME, QString( "isosurface" ) );
    m_properties["maingl"]->set( Fn::Property::ISO_VALUE, 80.0f, ds->properties( "maingl" )->get( Fn::Property::MIN ).toFloat(), ds->properties( "maingl" )->get( Fn::Property::MAX ).toFloat(), true );

    m_nX = m_properties["maingl"]->get( Fn::Property::NX ).toInt() - 1;
    m_nY = m_properties["maingl"]->get( Fn::Property::NY ).toInt() - 1;
    m_nZ = m_properties["maingl"]->get( Fn::Property::NZ ).toInt() - 1;

    m_dX = m_properties["maingl"]->get( Fn::Property::DX ).toFloat();
    m_dY = m_properties["maingl"]->get( Fn::Property::DY ).toFloat();
    m_dZ = m_properties["maingl"]->get( Fn::Property::DZ ).toFloat();

    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );

    QMutex* mutex = new QMutex();

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new IsoSurfaceThread( &m_scalarField, mutex, &m_i2pt3idVertices, &m_trivecTriangles, m_nX, m_nY, m_nZ, m_dX, m_dY, m_dZ, i ) );
    }

    generateSurface();
}

DatasetIsosurface::~DatasetIsosurface()
{
}

void DatasetIsosurface::generateSurface()
{
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->setIsoLevel( m_isoLevel );
    }
    // run threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->wait();
    }
    renameVerticesAndTriangles();
}

void DatasetIsosurface::renameVerticesAndTriangles()
{
    unsigned int nextID = 0;
    QMap<int, POINT3DID>::iterator mapIterator = m_i2pt3idVertices.begin();
    TRIANGLEVECTOR::iterator vecIterator = m_trivecTriangles.begin();

    TriangleMesh2* mesh = new TriangleMesh2( m_i2pt3idVertices.size(), m_trivecTriangles.size() );
    m_mesh.push_back( mesh );

    float xOff = 0.5f;
    float yOff = 0.5f;
    float zOff = 0.5f;

    // Rename vertices.
    while ( mapIterator != m_i2pt3idVertices.end() )
    {
        ( *mapIterator ).newID = nextID++;
        m_mesh[0]->addVertex( ( *mapIterator ).x + xOff, ( *mapIterator ).y + yOff, ( *mapIterator ).z + zOff );
        ++mapIterator;
    }

    // Now rename triangles.
    while ( vecIterator != m_trivecTriangles.end() )
    {
        ( *vecIterator ).pointID[ 0 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 0 ] ].newID;
        ( *vecIterator ).pointID[ 1 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 1 ] ].newID;
        ( *vecIterator ).pointID[ 2 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 2 ] ].newID;

        m_mesh[0]->addTriangle( ( *vecIterator ).pointID[ 0 ], ( *vecIterator ).pointID[ 1 ], ( *vecIterator ).pointID[ 2 ] );
        ++vecIterator;
    }
    m_mesh[0]->finalize();

    m_i2pt3idVertices.clear();
    m_trivecTriangles.clear();
}

void DatasetIsosurface::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh[0] );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }

    m_isoLevel = m_properties["maingl"]->get( Fn::Property::ISO_VALUE ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh[0];
        m_mesh.clear();
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh[0] );
    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, m_properties["maingl"] );
}
