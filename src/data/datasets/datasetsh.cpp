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
    //m_properties["maingl"].create( Fn::Property::D_SCALING, 1.0f, 0.0f, 2.0f, true );
    m_properties["maingl"].createBool( Fn::Property::D_MINMAX_SCALING, false, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_SCALING, 1.0f, 0.1f, 2.0f, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_HIDE_NEGATIVE_LOBES, false, "general" );
    m_properties["maingl"].createInt( Fn::Property::D_LOD, 0, 0, 4, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );

    m_properties["maingl"].createBool( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_AMBIENT,   0.5f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SHININESS, 1.0f, 0.0f, 200.0f, "light" );

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
    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( &m_data, properties( target ).get( Fn::Property::D_NX ).toInt(),
                                              properties( target ).get( Fn::Property::D_NY ).toInt(),
                                              properties( target ).get( Fn::Property::D_NZ ).toInt(),
                                              properties( target ).get( Fn::Property::D_DX ).toFloat(),
                                              properties( target ).get( Fn::Property::D_DY ).toFloat(),
                                              properties( target ).get( Fn::Property::D_DZ ).toFloat() );
        m_renderer->setModel( Models::g() );
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
        m_renderer = new SHRenderer( &m_data, properties( "maingl" ).get( Fn::Property::D_NX ).toInt(),
                                              properties( "maingl" ).get( Fn::Property::D_NY ).toInt(),
                                              properties( "maingl" ).get( Fn::Property::D_NZ ).toInt(),
                                              properties( "maingl" ).get( Fn::Property::D_DX ).toFloat(),
                                              properties( "maingl" ).get( Fn::Property::D_DY ).toFloat(),
                                              properties( "maingl" ).get( Fn::Property::D_DZ ).toFloat() );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }
    return m_renderer->getMesh();
}
