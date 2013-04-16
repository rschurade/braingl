/*
 * datasetisosurface.cpp
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */
#include "datasetisosurface.h"

#include "datasetscalar.h"
#include "isosurfacethread.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/glfunctions.h"
#include "../../gui/gl/meshrenderer.h"

#include <QString>

DatasetIsosurface::DatasetIsosurface( DatasetScalar* ds ) :
        Dataset( QString( "isosurface" ), Fn::DatasetType::MESH_ISOSURFACE ),
        m_mesh( 0 ),
        m_renderer( 0 ),
        m_oldIsoValue( -1 )
{
    m_scalarField = *(ds->getData() );

    m_properties.set( Fn::Property::NX, ds->properties()->get( Fn::Property::NX ).toInt() );
    m_properties.set( Fn::Property::NY, ds->properties()->get( Fn::Property::NY ).toInt() );
    m_properties.set( Fn::Property::NZ, ds->properties()->get( Fn::Property::NZ ).toInt() );
    m_properties.set( Fn::Property::DX, ds->properties()->get( Fn::Property::DX ).toFloat() );
    m_properties.set( Fn::Property::DY, ds->properties()->get( Fn::Property::DY ).toFloat() );
    m_properties.set( Fn::Property::DZ, ds->properties()->get( Fn::Property::DZ ).toFloat() );

    m_properties.set( Fn::Property::DIM, 0 );
    m_properties.set( Fn::Property::CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties.set( Fn::Property::TYPE, (int)Fn::DatasetType::MESH_ISOSURFACE );
    m_properties.set( Fn::Property::NAME, QString( "isosurface" ) );
    m_properties.set( Fn::Property::ISO_VALUE, 80.0f, ds->properties()->get( Fn::Property::MIN ).toFloat(), ds->properties()->get( Fn::Property::MAX ).toFloat(), true );

    m_properties.set( Fn::Property::COLOR, QColor( 255, 255, 255 ), true );
    m_properties.set( Fn::Property::COLORMODE, { "user", "mri" }, 0, true );
    m_properties.set( Fn::Property::COLORMAP, 1 );
    m_properties.set( Fn::Property::MIN, 0.0f );
    m_properties.set( Fn::Property::MAX, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MIN, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::SELECTED_MAX, 1.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );

    m_properties.set( Fn::Property::PAINTMODE, { "off", "paint", "erase" }, 0, true );
    m_properties.set( Fn::Property::PAINTSIZE, 2.f, 1.f, 100.f, true );
    m_properties.set( Fn::Property::PAINTCOLOR, QColor( 255, 0, 0 ), true );


    m_nX = m_properties.get( Fn::Property::NX ).toInt() - 1;
    m_nY = m_properties.get( Fn::Property::NY ).toInt() - 1;
    m_nZ = m_properties.get( Fn::Property::NZ ).toInt() - 1;

    m_dX = m_properties.get( Fn::Property::DX ).toFloat();
    m_dY = m_properties.get( Fn::Property::DY ).toFloat();
    m_dZ = m_properties.get( Fn::Property::DZ ).toFloat();

    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );

    QMutex* mutex = new QMutex();

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new IsoSurfaceThread( &m_scalarField, mutex, &m_i2pt3idVertices, &m_trivecTriangles, m_nX, m_nY, m_nZ, m_dX, m_dY, m_dZ, i ) );
    }
}

DatasetIsosurface::~DatasetIsosurface()
{
}

TriangleMesh2* DatasetIsosurface::getMesh()
{
    return m_mesh;
}

void DatasetIsosurface::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }

    m_isoLevel = m_properties.get( Fn::Property::ISO_VALUE ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh;
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh );
    }

    m_renderer->draw( pMatrix, mvMatrix, &m_properties );
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

void DatasetIsosurface::mousePick( int pickId, QVector3D pos )
{
   if ( pickId == 0 )
   {
       return;
   }

   int paintMode = m_properties.get( Fn::Property::PAINTMODE ).toInt();
   if (  paintMode != 0 )
   {
       QColor color;
       if ( paintMode == 1 )
       {
           color = m_properties.get( Fn::Property::PAINTCOLOR ).value<QColor>();
       }
       else if ( paintMode == 2 )
       {
           color = m_properties.get( Fn::Property::COLOR ).value<QColor>();
       }

       QVector<int>picked = m_mesh->pick( pos, m_properties.get( Fn::Property::PAINTSIZE ).toFloat() );

       if ( picked.size() > 0 )
       {
           m_renderer->beginUpdateColor();
           for ( int i = 0; i < picked.size(); ++i )
           {
               m_renderer->updateColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
               m_mesh->setVertexColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
           }
           m_renderer->endUpdateColor();
       }
   }
}
