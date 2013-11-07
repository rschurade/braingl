/*
 * dataset3d.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "datasetdwi.h"

#include "../models.h"

#include <QDebug>

DatasetDWI::DatasetDWI( QDir filename, QVector<ColumnVector> data, QVector<float> b0Data, QVector<float> bvals, QVector<QVector3D> bvecs, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_DWI, header ),
    m_data( data ),
    m_b0Data( b0Data ),
    m_bvals( bvals ),
    m_bvecs( bvecs )
{
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 0, 0, (int)Fn::ColormapEnum::NONE - 1, "color" );
    m_properties["maingl"]->create( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );

    examineDataset();

    m_properties["maingl"]->create( Fn::Property::D_HAS_TEXTURE, true );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
}

DatasetDWI::~DatasetDWI()
{
    m_data.clear();
    m_b0Data.clear();
    m_bvals.clear();
    m_bvecs.clear();
}

QVector<ColumnVector>* DatasetDWI::getData()
{
    return &m_data;
}

QVector<float>* DatasetDWI::getB0Data()
{
    return &m_b0Data;
}

QVector<float> DatasetDWI::getBvals()
{
    return m_bvals;
}

QVector<QVector3D> DatasetDWI::getBvecs()
{
    return m_bvecs;
}

void DatasetDWI::examineDataset()
{
    int datatype = m_properties["maingl"]->get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();


    int dim = m_data.at( 0 ).Nrows();
    m_properties["maingl"]->create( Fn::Property::D_DIM, dim );
    int size = nx * ny * nz * dim;

    float min = 0;
    float max = 0;

    if ( datatype == DT_UNSIGNED_CHAR )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(unsigned char) ) );
        m_properties["maingl"]->create( Fn::Property::D_MIN, 0 );
        m_properties["maingl"]->create( Fn::Property::D_MAX, 255 );    }
        min = 0;
        max = 255;

    if ( datatype == DT_SIGNED_SHORT )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(signed short) ) );

        max = -32767;
        min = 32768;

        for ( int i = 0; i < m_data.size(); ++i )
        {
            for ( int k = 1; k < dim + 1; ++k )
            {
                min = qMin( min, (float)( m_data[i]( k ) ) );
                max = qMax( max, (float)( m_data[i]( k ) ) );
            }
        }
        m_properties["maingl"]->create( Fn::Property::D_MIN, min );
        m_properties["maingl"]->create( Fn::Property::D_MAX, max );
    }

    if ( datatype == DT_FLOAT )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );

        max = -32767;
        min = 32768;

        for ( int i = 0; i < m_data.size(); ++i )
        {
            for ( int k = 1; k < dim + 1; ++k )
            {
                min = qMin( min, (float)( m_data[i]( k ) ) );
                max = qMax( max, (float)( m_data[i]( k ) ) );
            }
        }
        m_properties["maingl"]->create( Fn::Property::D_MIN, min );
        m_properties["maingl"]->create( Fn::Property::D_MAX, max );
    }
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );


    m_properties["maingl"]->create( Fn::Property::D_SELECTED_TEXTURE, 0, 0, dim - 1, "general" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_TEXTURE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectTexture() ) );
}

void DatasetDWI::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int frame = m_properties["maingl"]->get( Fn::Property::D_SELECTED_TEXTURE ).toInt() + 1;

    float max = m_properties["maingl"]->get( Fn::Property::D_MAX ).toFloat();

    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        //unsigned int tmp = (double)i / (double)(nx * ny * nz) * 256 * 256 * 256 * 256;
        unsigned int tmp = ( m_data[i]( frame) / max ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetDWI::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
}

QString DatasetDWI::getValueAsString( int x, int y, int z )
{
   float data = m_data[ getId( x, y, z ) ]( m_properties["maingl"]->get( Fn::Property::D_SELECTED_TEXTURE ).toInt() + 1 );

    return QString::number( data );
}

void DatasetDWI::selectTexture()
{
    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
}
