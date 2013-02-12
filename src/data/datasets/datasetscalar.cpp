/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: Ralph Schurade
 */
#include "datasetscalar.h"

#include <QtCore/QDebug>

DatasetScalar::DatasetScalar( QString filename, QVector<float> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_SCALAR, header ), m_data( data )
{
    m_properties.set( Fn::Property::COLORMAP, 0, 0, (int)Fn::Colormap::NONE - 1, true );
    m_properties.set( Fn::Property::INTERPOLATION, false, true );
    m_properties.set( Fn::Property::ALPHA, 1.0f, 0.0, 1.0, true );
    m_properties.set( Fn::Property::DIM, 1 );
    m_properties.set( Fn::Property::HAS_TEXTURE, true );

    examineDataset();
}

DatasetScalar::~DatasetScalar()
{
    m_data.clear();
    glDeleteTextures( 1, &m_textureGLuint );
}

QVector<float>* DatasetScalar::getData()
{
    return &m_data;
}


void DatasetScalar::examineDataset()
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[i] );
        max = qMax( max, m_data[i] );
    }

    m_properties.set( Fn::Property::SIZE, static_cast<int>( size * sizeof(float) ) );
    m_properties.set( Fn::Property::MIN, min );
    m_properties.set( Fn::Property::MAX, max );
    m_properties.set( Fn::Property::SELECTED_MIN, min, min, max, true );
    m_properties.set( Fn::Property::SELECTED_MAX, max, min, max, true );
    m_properties.set( Fn::Property::LOWER_THRESHOLD, min + (max-min)/1000., min, max, true );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, max, min, max, true );


    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties.get( Fn::Property::NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }
}

void DatasetScalar::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();

    float max = m_properties.get( Fn::Property::MAX ).toFloat();

    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        //unsigned int tmp = (double)i / (double)(nx * ny * nz) * 256 * 256 * 256 * 256;
        unsigned int tmp = ( m_data[i] / max ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetScalar::flipX()
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();

    QVector<float> newData;

    for ( int z = 0; z < nz; ++z )
    {
        for ( int y = 0; y < ny; ++y )
        {
            for ( int x = nx - 1; x >= 0; --x )
            {
                newData.push_back( m_data[x + y * nx + z * nx * ny] );
            }
        }
    }

    m_header->qoffset_x = 0.0;

    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
    m_header->qto_xyz.m[0][3] = 0.0;
    m_header->sto_xyz.m[0][3] = 0.0;

    m_data.clear();
    m_data = newData;
}

void DatasetScalar::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel )
{
}

QString DatasetScalar::getValueAsString( int x, int y, int z )
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    float data = m_data[x + y * nx + z * nx * ny];
    return QString::number( data );
}
