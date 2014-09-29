/*
 * dataset3d.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "datasetsh.h"
#include "../models.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/shrenderer.h"

DatasetSH::DatasetSH( QDir filename, std::vector<ColumnVector> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_SH, header ), m_data( data ), m_renderer( 0 )
{
    m_properties["maingl"].createInt( Fn::Property::D_OFFSET, 0, -1, 1, "general" );
    m_properties["maingl"].createInt( Fn::Property::D_LOD, 0, 0, 5, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_MINMAX_SCALING, false, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_SCALING, 1.0f, 0.1f, 2.0f );
    m_properties["maingl"].createBool( Fn::Property::D_HIDE_NEGATIVE_LOBES, false, "general" );

    m_properties["maingl"].createBool( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_AMBIENT,   0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_DIFFUSE,   0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_AMBIENT,   2.0f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_DIFFUSE,   1.5f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SPECULAR,  0.5f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SHININESS, 150.0f, 0.0f, 200.0f, "light" );

    examineDataset();

    PropertyGroup props2( ( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"].getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
}

DatasetSH::~DatasetSH()
{
    m_data.clear();
}

void DatasetSH::examineDataset()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();
    int size = nx * ny * nz;
    int dim = m_data.at( 0 ).Nrows();

    m_properties["maingl"].createInt( Fn::Property::D_SIZE, static_cast<int>( dim * size * sizeof(float) ) );

    m_properties["maingl"].createFloat( Fn::Property::D_LOWER_THRESHOLD, m_properties["maingl"].get( Fn::Property::D_MIN ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_UPPER_THRESHOLD, m_properties["maingl"].get( Fn::Property::D_MAX ).toFloat() );

    m_properties["maingl"].createInt( Fn::Property::D_LOD, 2 );
    m_properties["maingl"].createInt( Fn::Property::D_ORDER, 0 );
    m_properties["maingl"].createInt( Fn::Property::D_RENDER_SLICE, 1 );
    m_properties["maingl"].createFloat( Fn::Property::D_SCALING, 1.0f );
    m_properties["maingl"].createInt( Fn::Property::D_DIM, dim );

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for ( int i = 0; i < size; ++i )
    {
        ColumnVector d = m_data.at( i );
        for ( int j = 1; j < dim + 1; ++j )
        {
            min = qMin( min, (float) d( j ) );
            max = qMax( max, (float) d( j ) );
        }
    }

    m_properties["maingl"].createFloat( Fn::Property::D_MIN, min );
    m_properties["maingl"].createFloat( Fn::Property::D_MAX, max );
}

void DatasetSH::createTexture()
{
}

std::vector<ColumnVector>* DatasetSH::getData()
{
    return &m_data;
}

void DatasetSH::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_resetRenderer )
    {
        if ( m_renderer != 0 )
        {
            delete m_renderer;
            m_renderer = 0;
        }
        m_resetRenderer = false;
    }

    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( &m_data );
        m_renderer->init();
    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetSH::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

TriangleMesh2* DatasetSH::getMeshFromCurrent()
{
    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( &m_data );
        m_renderer->init();
    }
    return m_renderer->getMesh();
}

void DatasetSH::flipX()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    for ( int x = 0; x < nx / 2; ++x )
    {
        for ( int y = 0; y < ny; ++y )
        {
            for ( int z = 0; z < nz; ++z )
            {
                ColumnVector tmp = m_data[getId( x, y, z) ];
                m_data[getId( x, y, z) ] = m_data[getId( nx - x, y, z) ];
                m_data[getId( nx - x, y, z) ] = tmp;
            }
        }
    }
    m_resetRenderer = true;
    Models::g()->submit();
}

void DatasetSH::flipY()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    for ( int x = 0; x < nx; ++x )
    {
        for ( int y = 0; y < ny / 2; ++y )
        {
            for ( int z = 0; z < nz; ++z )
            {
                ColumnVector tmp = m_data[getId( x, y, z) ];
                m_data[getId( x, y, z) ] = m_data[getId( x, ny - y, z) ];
                m_data[getId( x, ny - y, z) ] = tmp;
            }
        }
    }
    m_resetRenderer = true;
    Models::g()->submit();
}

void DatasetSH::flipZ()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    for ( int x = 0; x < nx; ++x )
    {
        for ( int y = 0; y < ny; ++y )
        {
            for ( int z = 0; z < nz / 2; ++z )
            {
                ColumnVector tmp = m_data[getId( x, y, z) ];
                m_data[getId( x, y, z) ] = m_data[getId( x, y, nz - z) ];
                m_data[getId( x, y, nz - z) ] = tmp;
            }
        }
    }
    m_resetRenderer = true;
    Models::g()->submit();
}
