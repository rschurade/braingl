/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <limits>

#include <QtCore/QDebug>

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( QString filename, void* data ) :
    DatasetNifti( filename, FNDT_NIFTI_SCALAR, data )
{
}

DatasetScalar::~DatasetScalar()
{
}

void DatasetScalar::examineDataset()
{
    if ( getDatatype() == DT_UNSIGNED_CHAR )
    {
        unsigned char* data = reinterpret_cast< unsigned char* >( m_data );

        unsigned char min = 255;
        unsigned char max = 0;

        int size = nx * ny * nz;
        for ( size_t i = 0; i < size; ++i )
        {
            min = qMin( min, data[ i ] );
            max = qMax( max, data[ i ] );
        }
        m_min = static_cast< float >( min );
        m_max = static_cast< float >( max );

        m_properties["size"] = size * sizeof( unsigned char );
    }
    if ( getDatatype() == DT_SIGNED_SHORT )
    {
        short* data = reinterpret_cast< short* >( m_data );

        short min = 65535;
        short max = 0;

        int size = nx * ny * nz;
        for ( size_t i = 0; i < size; ++i )
        {
            min = qMin( min, data[ i ] );
            max = qMax( max, data[ i ] );
        }
        m_min = static_cast< float >( min );
        m_max = static_cast< float >( max );

        m_properties["size"] = size * sizeof( short );
    }
    if ( getDatatype() == DT_FLOAT )
    {
        float* data = reinterpret_cast< float* >( m_data );

        m_min = std::numeric_limits<float>::max();
        m_max = 0;

        int size = nx * ny * nz;
        for ( size_t i = 0; i < size; ++i )
        {
            m_min = qMin( m_min, data[ i ] );
            m_max = qMax( m_max, data[ i ] );
        }

        m_properties["size"] = size * sizeof( float );
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

    if ( getDatatype() == DT_UNSIGNED_CHAR )
    {
        glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_data );
    }

    if ( getDatatype() == DT_SIGNED_SHORT )
    {
        short* data = reinterpret_cast< short* >( m_data );

        int mult = 65535 / m_max;
        short* tmpData = new short[nx*ny*nz];
        for ( int i = 0; i < nx*ny*nz; ++i )
        {
            tmpData[i] = data[i] * mult;
        }

        glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_SHORT, tmpData );
        delete[] tmpData;
    }

    if ( getDatatype() == DT_FLOAT )
    {
        glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_FLOAT, m_data );
    }
}
