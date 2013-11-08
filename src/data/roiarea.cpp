/*
 * roiarea.cpp
 *
 * Created on: 02.02.2013
 * @author Ralph Schurade
 */
#include "roiarea.h"

#include "models.h"
#include "datasets/isosurfacethread.h"

#include "mesh/trianglemesh2.h"

#include "../gui/gl/meshrenderer.h"
#include "../gui/gl/glfunctions.h"

#include <QAbstractItemModel>

ROIArea::ROIArea( QVector<float> data, nifti_image* header ) :
    ROI( QString("new roi") + QString::number( ROI::m_count++ ) ),
    m_data( data ),
    m_header( header ),
    m_renderer( 0 ),
    m_mesh( 0 )
{
    m_max = 0;
    for ( int i = 0; i < data.size(); ++i )
    {
        m_max = qMax( m_max, data[i] );
    }

    m_properties.create( Fn::Property::R_RENDER, true, "general" );
    m_properties.create( Fn::Property::R_NEG, false, "general" );
    m_properties.create( Fn::Property::R_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties.create( Fn::Property::R_ALPHA, 0.4f, 0.f, 1.f, "general" );
    m_properties.create( Fn::Property::R_PICK_ID, 0 );
    m_properties.create( Fn::Property::R_SHAPE, 10 );
    m_properties.create( Fn::Property::R_THRESHOLD, m_max / 100.f, 0.0f, m_max, "general" );
    m_isoLevel = m_max;
    m_oldIsoValue = m_max;

    m_properties.create( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties.create( Fn::Property::D_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties.create( Fn::Property::D_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );
    m_properties.create( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties.create( Fn::Property::D_MATERIAL_AMBIENT,   0.5f, 0.0f, 10.0f, "light" );
    m_properties.create( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties.create( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties.create( Fn::Property::D_MATERIAL_SHININESS, 1.0f, 0.0f, 200.0f, "light" );



    m_properties.create( Fn::Property::R_NX, m_header->nx );
    m_properties.create( Fn::Property::R_NY, m_header->ny );
    m_properties.create( Fn::Property::R_NZ, m_header->nz );
    m_properties.create( Fn::Property::R_DX, m_header->dx );
    m_properties.create( Fn::Property::R_DY, m_header->dy );
    m_properties.create( Fn::Property::R_DZ, m_header->dz );

    m_properties.create( Fn::Property::D_START_INDEX, 0 );
    m_properties.create( Fn::Property::D_END_INDEX, 0 );

    m_properties.create( Fn::Property::R_PICK_ID, (int)GLFunctions::getPickIndex() );

    connect( Models::g(), SIGNAL(  dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );

    m_nX = m_header->nx - 1;
    m_nY = m_header->ny - 1;
    m_nZ = m_header->nz - 1;

    m_dX = m_header->dx;
    m_dY = m_header->dy;
    m_dZ = m_header->dz;

    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );

    QMutex* mutex = new QMutex();

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new IsoSurfaceThread( &m_data, mutex, &m_i2pt3idVertices, &m_trivecTriangles, m_nX, m_nY, m_nZ, m_dX, m_dY, m_dZ, i ) );
    }

    generateSurface();
}

ROIArea::~ROIArea()
{
}

QVector<float>* ROIArea::data()
{
    return &m_data;
}

void ROIArea::globalChanged()
{
}

void ROIArea::generateSurface()
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

void ROIArea::renameVerticesAndTriangles()
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

    m_properties.set( Fn::Property::D_END_INDEX, m_mesh->numTris() );

    m_i2pt3idVertices.clear();
    m_trivecTriangles.clear();
}

void ROIArea::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }

    m_isoLevel = m_properties.get( Fn::Property::R_THRESHOLD ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh;
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh );

    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, m_properties );
}

