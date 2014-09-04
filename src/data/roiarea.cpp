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

ROIArea::ROIArea( std::vector<float> data, PropertyGroup& props ) :
    ROI( QString("new roi") + QString::number( ROI::m_count++ ) ),
    m_data( data ),
    m_renderer( 0 ),
    m_mesh( 0 )
{
    m_max = 0;
    for ( unsigned int i = 0; i < data.size(); ++i )
    {
        m_max = qMax( m_max, data[i] );
    }

    m_properties.createBool( Fn::Property::D_RENDER, true, "general" );
    m_properties.createBool( Fn::Property::D_NEG, false, "general" );
    m_properties.createColor( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties.createFloat( Fn::Property::D_ALPHA, 0.4f, 0.f, 1.f, "general" );
    m_properties.createInt( Fn::Property::D_PICK_ID, 0 );
    m_properties.createInt( Fn::Property::D_SHAPE, 10 );
    m_properties.createFloat( Fn::Property::D_THRESHOLD, m_max / 100.f, 0.0f, m_max, "general" );
    m_properties.createDir( Fn::Property::D_FILENAME, QDir( "" ) );
    m_isoLevel = m_max;
    m_oldIsoValue = m_max;

    m_properties.createBool( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties.createFloat( Fn::Property::D_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties.createFloat( Fn::Property::D_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );
    m_properties.createFloat( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties.createFloat( Fn::Property::D_MATERIAL_AMBIENT,   0.5f, 0.0f, 10.0f, "light" );
    m_properties.createFloat( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties.createFloat( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties.createFloat( Fn::Property::D_MATERIAL_SHININESS, 1.0f, 0.0f, 200.0f, "light" );

    m_properties.createFloat( Fn::Property::D_NX, props.get( Fn::Property::D_NX ).toInt() );
    m_properties.createFloat( Fn::Property::D_NY, props.get( Fn::Property::D_NY ).toInt() );
    m_properties.createFloat( Fn::Property::D_NZ, props.get( Fn::Property::D_NZ ).toInt() );
    m_properties.createFloat( Fn::Property::D_DX, props.get( Fn::Property::D_DX ).toFloat() );
    m_properties.createFloat( Fn::Property::D_DY, props.get( Fn::Property::D_DY ).toFloat() );
    m_properties.createFloat( Fn::Property::D_DZ, props.get( Fn::Property::D_DZ ).toFloat() );
    m_properties.createFloat( Fn::Property::D_ADJUST_X, props.get( Fn::Property::D_ADJUST_X ).toFloat() );
    m_properties.createFloat( Fn::Property::D_ADJUST_Y, props.get( Fn::Property::D_ADJUST_Y ).toFloat() );
    m_properties.createFloat( Fn::Property::D_ADJUST_Z, props.get( Fn::Property::D_ADJUST_Z ).toFloat() );

    m_properties.createInt( Fn::Property::D_START_INDEX, 0 );
    m_properties.createInt( Fn::Property::D_END_INDEX, 0 );

    m_properties.createInt( Fn::Property::D_PICK_ID, (int)GLFunctions::getPickIndex() );

    connect( Models::g(), SIGNAL(  dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );

    int nx = props.get( Fn::Property::D_NX ).toInt() - 1;
    int ny = props.get( Fn::Property::D_NY ).toInt() - 1;
    int nz = props.get( Fn::Property::D_NZ ).toInt() - 1;

    float dx = props.get( Fn::Property::D_DX ).toFloat();
    float dy = props.get( Fn::Property::D_DY ).toFloat();
    float dz = props.get( Fn::Property::D_DZ ).toFloat();

    m_nPointsInXDirection = ( nx + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( ny + 1 );

    QMutex* mutex = new QMutex();

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new IsoSurfaceThread( &m_data, mutex, &m_i2pt3idVertices, &m_trivecTriangles, nx, ny, nz, dx, dy, dz, i ) );
    }

    generateSurface();
}

ROIArea::~ROIArea()
{
}

std::vector<float>* ROIArea::data()
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


    float plusX = 0; //m_properties.get( Fn::Property::D_ADJUST_X ).toFloat();
    float plusY = 0; //m_properties.get( Fn::Property::D_ADJUST_Y ).toFloat();
    float plusZ = 0; //m_properties.get( Fn::Property::D_ADJUST_Z ).toFloat();

    // Rename vertices.
    while ( mapIterator != m_i2pt3idVertices.end() )
    {
        ( *mapIterator ).newID = nextID++;
        //m_mesh->addVertex( ( *mapIterator ).x + xOff, ( *mapIterator ).y + yOff, ( *mapIterator ).z + zOff );
        m_mesh->addVertex( ( *mapIterator ).x + plusX, ( *mapIterator ).y + plusY, ( *mapIterator ).z + plusZ );
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
        m_renderer->init();
    }

    m_isoLevel = m_properties.get( Fn::Property::D_THRESHOLD ).toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh;
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh );

    }

    if ( m_properties.get( Fn::Property::D_RENDER).toBool() )
    {
        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, m_properties );
    }
}

