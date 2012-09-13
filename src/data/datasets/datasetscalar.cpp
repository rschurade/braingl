/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <limits>

#include <QtCore/QDebug>

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( QString filename, QVector<float> data, nifti_image* header ) :
    DatasetNifti( filename, FNDT_NIFTI_SCALAR, header ),
    m_data( data )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;

    examineDataset();
}

DatasetScalar::~DatasetScalar()
{
    m_data.clear();
    m_properties.clear();
    glDeleteTextures( 1, &m_textureGLuint );
}

void DatasetScalar::examineDataset()
{
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[ i ] );
        max = qMax( max, m_data[ i ] );
    }

    m_properties["min"] = min;
    m_properties["max"] = max;

    m_properties["size"] = static_cast<int>( size * sizeof( float ) );

    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["name"].toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
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

    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();

    float max = m_properties["max"].toFloat();

    float* tmpData = new float[nx*ny*nz];
    for ( int i = 0; i < nx*ny*nz; ++i )
    {
        tmpData[i] = m_data[i] / max ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_FLOAT, tmpData );
    delete[] tmpData;

}

QVector<float> DatasetScalar::getData()
{
    return m_data;
}

void DatasetScalar::flipX()
{
    int xDim = m_properties["nx"].toInt();
    int yDim = m_properties["ny"].toInt();
    int zDim = m_properties["nz"].toInt();

    QVector<float> newData;

    for( int z = 0; z < zDim; ++z )
    {
        for( int y = 0; y < yDim; ++y )
        {
            for( int x = xDim -1; x >= 0; --x )
            {
                newData.push_back( m_data[ x + y * xDim + z * xDim * yDim ] );
            }
        }
    }

    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
    m_header->qto_xyz.m[0][3] = 0.0;
    m_header->sto_xyz.m[0][3] = 0.0;


    m_data.clear();
    m_data = newData;
}
