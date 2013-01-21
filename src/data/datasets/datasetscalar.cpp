/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( QString filename, QVector<float> data, nifti_image* header ) :
        DatasetNifti( filename, FNDT_NIFTI_SCALAR, header ), m_data( data )
{
    setProperty( FNPROP_ACTIVE, true );
    setProperty( FNPROP_COLORMAP, 0 );
    setProperty( FNPROP_INTERPOLATION, false );
    setProperty( FNPROP_ALPHA, 1.0f );
    setProperty( FNPROP_DIM, 1 );

    examineDataset();
}

DatasetScalar::~DatasetScalar()
{
    m_data.clear();
    glDeleteTextures( 1, &m_textureGLuint );
}

void DatasetScalar::examineDataset()
{
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    int nz = getProperty( FNPROP_NZ ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[i] );
        max = qMax( max, m_data[i] );
    }

    setProperty( FNPROP_SIZE, static_cast<int>( size * sizeof(float) ) );
    setProperty( FNPROP_MIN, min );
    setProperty( FNPROP_MAX, max );

    setProperty( FNPROP_LOWER_THRESHOLD, min );
    setProperty( FNPROP_UPPER_THRESHOLD, max );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << getProperty( FNPROP_NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
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

    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    int nz = getProperty( FNPROP_NZ ).toInt();

    float max = getProperty( FNPROP_MAX ).toFloat();

    float* tmpData = new float[nx * ny * nz];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        tmpData[i] = m_data[i] / max;
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
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    int nz = getProperty( FNPROP_NZ ).toInt();

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

void DatasetScalar::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore )
{
}

QString DatasetScalar::getValueAsString( int x, int y, int z )
{
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    float data = m_data[x + y * nx + z * nx * ny];
    return QString::number( data );
}
