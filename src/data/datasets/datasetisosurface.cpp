/*
 * datasetisosurface.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */
#include "datasetisosurface.h"

#include "datasetscalar.h"
#include "isosurfacethread.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/meshrenderer.h"

#include <QtCore/QString>

DatasetIsosurface::DatasetIsosurface( DatasetScalar* ds ) :
        Dataset( QString( "isosurface" ), Fn::DatasetType::MESH_ISOSURFACE ),
        m_mesh( 0 ),
        m_renderer( 0 ),
        m_oldIsoValue( -1 )
{
    m_scalarField = *(ds->getData() );

    m_properties.set( FNPROP_NX, ds->properties()->get( FNPROP_NX ).toInt() );
    m_properties.set( FNPROP_NY, ds->properties()->get( FNPROP_NY ).toInt() );
    m_properties.set( FNPROP_NZ, ds->properties()->get( FNPROP_NZ ).toInt() );
    m_properties.set( FNPROP_DX, ds->properties()->get( FNPROP_DX ).toFloat() );
    m_properties.set( FNPROP_DY, ds->properties()->get( FNPROP_DY ).toFloat() );
    m_properties.set( FNPROP_DZ, ds->properties()->get( FNPROP_DZ ).toFloat() );

    m_properties.set( FNPROP_DIM, 0 );
    m_properties.set( FNPROP_CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties.set( FNPROP_TYPE, (int)Fn::DatasetType::MESH_ISOSURFACE );
    m_properties.set( FNPROP_NAME, QString( "isosurface" ) );
    m_properties.set( FNPROP_ISO_VALUE, 80.0f, ds->properties()->get( FNPROP_MIN ).toFloat(), ds->properties()->get( FNPROP_MAX ).toFloat(), true );

    m_nX = m_properties.get( FNPROP_NX ).toInt() - 1;
    m_nY = m_properties.get( FNPROP_NY ).toInt() - 1;
    m_nZ = m_properties.get( FNPROP_NZ ).toInt() - 1;

    m_dX = m_properties.get( FNPROP_DX ).toFloat();
    m_dY = m_properties.get( FNPROP_DY ).toFloat();
    m_dZ = m_properties.get( FNPROP_DZ ).toFloat();

    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );

    QMutex* mutex = new QMutex();

    m_numThreads = QThread::idealThreadCount();

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new IsoSurfaceThread( &m_scalarField, mutex, &m_i2pt3idVertices, &m_trivecTriangles, m_nX, m_nY, m_nZ, m_dX, m_dY, m_dZ, i ) );
    }
}

DatasetIsosurface::~DatasetIsosurface()
{
}

void DatasetIsosurface::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh );
        m_renderer->setModel( model );
        m_renderer->init();
    }

    m_isoLevel = m_properties.get( FNPROP_ISO_VALUE ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh;
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh );
    }

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetIsosurface::getValueAsString( int x, int y, int z )
{
    return QString( "" );
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

    m_mesh = new TriangleMesh2( m_i2pt3idVertices.size(), m_trivecTriangles.size() );

    float xOff = 0.5f;
    float yOff = 0.5f;
    float zOff = 0.5f;

    // Rename vertices.
    while ( mapIterator != m_i2pt3idVertices.end() )
    {
        ( *mapIterator ).newID = nextID++;
        m_mesh->addVertex( ( *mapIterator ).x + xOff, ( *mapIterator ).y + yOff, ( *mapIterator ).z + zOff );
        ++mapIterator;
    }

    // Now rename triangles.
    while ( vecIterator != m_trivecTriangles.end() )
    {
        ( *vecIterator ).pointID[ 0 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 0 ] ].newID;
        ( *vecIterator ).pointID[ 1 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 1 ] ].newID;
        ( *vecIterator ).pointID[ 2 ] = m_i2pt3idVertices[ ( *vecIterator ).pointID[ 2 ] ].newID;

        m_mesh->addTriangle( ( *vecIterator ).pointID[ 0 ], ( *vecIterator ).pointID[ 1 ], ( *vecIterator ).pointID[ 2 ] );
        ++vecIterator;
    }
    m_mesh->finalize();

    m_i2pt3idVertices.clear();
    m_trivecTriangles.clear();
}
