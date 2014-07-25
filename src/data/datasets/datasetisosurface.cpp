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

    m_properties["maingl"].createInt( Fn::Property::D_NX, ds->properties( "maingl" ).get( Fn::Property::D_NX ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NY, ds->properties( "maingl" ).get( Fn::Property::D_NY ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NZ, ds->properties( "maingl" ).get( Fn::Property::D_NZ ).toInt() );
    m_properties["maingl"].createFloat( Fn::Property::D_DX, ds->properties( "maingl" ).get( Fn::Property::D_DX ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_DY, ds->properties( "maingl" ).get( Fn::Property::D_DY ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_DZ, ds->properties( "maingl" ).get( Fn::Property::D_DZ ).toFloat() );

    m_properties["maingl"].createInt( Fn::Property::D_DIM, 1 );
    m_properties["maingl"].createInt( Fn::Property::D_CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties["maingl"].createInt( Fn::Property::D_TYPE, (int)Fn::DatasetType::MESH_ISOSURFACE );

    QString name = ds->properties( "maingl" ).get( Fn::Property::D_NAME ).toString();
    if ( !name.contains( "isosurface" ) )
    {
        name +=  " (isosurface)";
    }
    m_properties["maingl"].createString( Fn::Property::D_NAME, name );

    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_LOWER_X, false, "special" );
    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_LOWER_Y, false, "special" );
    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_LOWER_Z, false, "special" );
    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_HIGHER_X, false, "special" );
    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_HIGHER_Y, false, "special" );
    m_properties["maingl"].createBool( Fn::Property::D_MESH_CUT_HIGHER_Z, false, "special" );

    m_properties["maingl"].createInt( Fn::Property::D_START_INDEX, 0 );
    m_properties["maingl"].createInt( Fn::Property::D_END_INDEX, 0 );

    finalizeProperties();

    m_properties["maingl"].createFloat( Fn::Property::D_ISO_VALUE,
            ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat() * 0.3f,
            ds->properties( "maingl" ).get( Fn::Property::D_MIN ).toFloat(),
            ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat(), "general" );

    m_nX = m_properties["maingl"].get( Fn::Property::D_NX ).toInt() - 1;
    m_nY = m_properties["maingl"].get( Fn::Property::D_NY ).toInt() - 1;
    m_nZ = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt() - 1;

    m_dX = m_properties["maingl"].get( Fn::Property::D_DX ).toFloat();
    m_dY = m_properties["maingl"].get( Fn::Property::D_DY ).toFloat();
    m_dZ = m_properties["maingl"].get( Fn::Property::D_DZ ).toFloat();

    m_plusX = ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_X ).toFloat();
    m_plusY = ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Y ).toFloat();
    m_plusZ = ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Z ).toFloat();

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

std::vector<float>* DatasetIsosurface::getData()
{
    return &m_scalarField;
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

    m_properties["maingl"].set( Fn::Property::D_START_INDEX, 0 );
    m_properties["maingl"].set( Fn::Property::D_END_INDEX, m_mesh[0]->numTris() );
    m_properties["maingl2"].set( Fn::Property::D_START_INDEX, 0 );
    m_properties["maingl2"].set( Fn::Property::D_END_INDEX, m_mesh[0]->numTris() );
}

void DatasetIsosurface::renameVerticesAndTriangles()
{
    unsigned int nextID = 0;
    QMap<int, POINT3DID>::iterator mapIterator = m_i2pt3idVertices.begin();
    TRIANGLEVECTOR::iterator vecIterator = m_trivecTriangles.begin();

    TriangleMesh2* mesh = new TriangleMesh2( m_i2pt3idVertices.size(), m_trivecTriangles.size() );
    m_mesh.push_back( mesh );

//    float xOff = 0.5f;
//    float yOff = 0.5f;
//    float zOff = 0.5f;
    // Rename vertices.
    while ( mapIterator != m_i2pt3idVertices.end() )
    {
        ( *mapIterator ).newID = nextID++;
        //m_mesh[0]->addVertex( ( *mapIterator ).x + xOff, ( *mapIterator ).y + yOff, ( *mapIterator ).z + zOff );
        m_mesh[0]->addVertex( ( *mapIterator ).x + m_plusX, ( *mapIterator ).y + m_plusY, ( *mapIterator ).z + m_plusZ );
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

void DatasetIsosurface::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh[0] );
        m_renderer->init();
    }

    m_isoLevel = properties( "maingl" ).get( Fn::Property::D_ISO_VALUE ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh[0];
        m_mesh.clear();
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh[0] );
    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetIsosurface::getSaveFilter()
{
    return QString( "niftii (*.nii.gz);;Mesh binary (*.vtk);; Mesh ascii (*.asc);; Mesh 1D data (*.1D);; Mesh rgb data (*.rgb);; Mesh obj (*.obj);; Mesh VRML (*.wrl);; Mesh JSON (*.json);; all files (*.*)" );
}

QString DatasetIsosurface::getDefaultSuffix()
{
    return QString( "nii.gz" );
}
