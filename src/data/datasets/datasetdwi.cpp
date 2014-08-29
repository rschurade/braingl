/*
 * dataset3d.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "datasetdwi.h"
#include "datasetscalar.h"

#include "../models.h"

#include "../../gui/gl/glfunctions.h"

#include <QDebug>

DatasetDWI::DatasetDWI( QDir fileName, std::vector<float>* data, std::vector<float> bvals, std::vector<QVector3D> bvecs, nifti_image* header ) :
    DatasetNifti( fileName, Fn::DatasetType::NIFTI_DWI, header ),
    m_bvals( bvals ),
    m_bvecs( bvecs ),
    m_isOK( true )
{
    int numB0 = 0;
    std::vector<float> bvals2;
    for ( unsigned int i = 0; i < bvals.size(); ++i )
    {
        if ( bvals[i] > 100 )
        {
            bvals2.push_back( bvals[i] );
        }
        else
        {
            ++numB0;
        }
    }
    qDebug() << "num b0:" << numB0;

    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    unsigned int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];
    int numData = dim - numB0;
    qDebug() << "num data:" << numData;

    if ( numData > dim || bvals2.size() != bvecs.size() )
    {
        qCritical() << "*** ERROR *** parsing bval and bvec files!";
        m_isOK = false;
    }
    try
    {
        m_data.resize( blockSize * numData );
    }
    catch ( std::bad_alloc& )
    {
        qCritical() << "*** error *** failed to allocate memory for dataset";
        m_isOK = false;
    }

    m_b0Data.resize( blockSize );
    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        int dataIndex = 0;
        for ( int j = 0; j < dim; ++j )
        {
            if ( bvals[j] > 100 )
            {
                m_data[ blockSize * dataIndex  + i ] = data->at( j * blockSize + i );
                ++dataIndex;
            }
            else
            {
                m_b0Data[i] += data->at( j * blockSize + i ) / numB0;
            }
        }
    }

    QString b0fn = fileName.path();
    if ( fileName.path().endsWith( ".nii.gz" ) )
    {
        b0fn.replace( ".nii.gz", "_b0.nii.gz" );
    }
    if ( fileName.path().endsWith( ".nii" ) )
    {
        b0fn.replace( ".nii", "_b0.nii" );
    }

    nifti_image* b0Hdr = nifti_copy_nim_info( m_header );
    DatasetScalar* datasetB0 = new DatasetScalar( b0fn, m_b0Data, b0Hdr );
    Models::addDataset( datasetB0 );

    m_properties["maingl"].createInt( Fn::Property::D_COLORMAP, 0, 0, (int)Fn::ColormapEnum::NONE - 1, "color" );
    m_properties["maingl"].createBool( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl"].createInt( Fn::Property::D_DIM, numData );

    examineDataset();

    m_properties["maingl"].createBool( Fn::Property::D_HAS_TEXTURE, true );

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"].getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
}

DatasetDWI::~DatasetDWI()
{
    m_data.clear();
    std::vector<float>().swap( m_data );
    m_b0Data.clear();
    std::vector<float>().swap( m_b0Data );
    m_bvals.clear();
    std::vector<float>().swap( m_bvals );
    m_bvecs.clear();
    std::vector<QVector3D>().swap( m_bvecs );
}

std::vector<float>* DatasetDWI::getData()
{
    return &m_data;
}

std::vector<float>* DatasetDWI::getB0Data()
{
    return &m_b0Data;
}

std::vector<float> DatasetDWI::getBvals()
{
    return m_bvals;
}

std::vector<QVector3D> DatasetDWI::getBvecs()
{
    return m_bvecs;
}

void DatasetDWI::examineDataset()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();


    int dim = m_properties["maingl"].get( Fn::Property::D_DIM ).toInt();

    int size = nx * ny * nz * dim;

    float min = std::numeric_limits<float>::max();
    float max = 0;

    m_properties["maingl"].createInt( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );

    for ( unsigned int i = 0; i < m_data.size(); ++i )
    {
        min = qMin( min, m_data[i] ) ;
        max = qMax( max, m_data[i] ) ;
    }
    m_properties["maingl"].createInt( Fn::Property::D_MIN, min );
    m_properties["maingl"].createInt( Fn::Property::D_MAX, max );

    m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );

    m_properties["maingl"].createInt( Fn::Property::D_SELECTED_TEXTURE, 0, 0, dim - 1, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_TEXTURE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectTexture() ) );
}

void DatasetDWI::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    int frame = m_properties["maingl"].get( Fn::Property::D_SELECTED_TEXTURE ).toInt();

    float max = m_properties["maingl"].get( Fn::Property::D_MAX ).toFloat();
    int blockSize = nx * ny * nz;


    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        //unsigned int tmp = (double)i / (double)(nx * ny * nz) * 256 * 256 * 256 * 256;
        unsigned int tmp = ( m_data[frame * blockSize + i] / max ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    GLFunctions::f->glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetDWI::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
}

QString DatasetDWI::getValueAsString( int x, int y, int z )
{
   return "";
}

void DatasetDWI::selectTexture()
{
    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
}

void DatasetDWI::flipX()
{

}

void DatasetDWI::flipY()
{

}

void DatasetDWI::flipZ()
{

}
