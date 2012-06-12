/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "datasetdwi.h"

DatasetDWI::DatasetDWI( QString filename, void* data, void* b0Data, QVector<int>bvals, QVector<QVector3D>bvecs ) :
        DatasetNifti( filename, FNDT_NIFTI_DWI, data ),
        m_b0Data( b0Data ),
        m_bvals( bvals ),
        m_bvecs( bvecs )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;
}

DatasetDWI::~DatasetDWI()
{
}

void DatasetDWI::examineDataset()
{
    int type = getProperty( "datatype" ).toInt();
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int dim = getProperty( "nt" ).toInt();
    int size = nx * ny * nz * dim;

    if ( type == DT_UNSIGNED_CHAR )
    {
        m_properties["size"] = static_cast<int>( size * sizeof( unsigned char ) );

        m_properties["min"] = 0;
        m_properties["max"] = 255;
    }


    if ( type == DT_SIGNED_SHORT )
    {
        m_properties["size"] = static_cast<int>( size * sizeof( unsigned char ) );

        short min = -32767;
        short max = 32768;
        short* data = reinterpret_cast< short* >( m_b0Data );

        int size = nx * ny * nz;
        for ( size_t i = 0; i < size; ++i )
        {
            min = qMin( min, data[ i ] );
            max = qMax( max, data[ i ] );
        }
        m_properties["min"] = static_cast< float >( min );
        m_properties["max"] = static_cast< float >( max );

    }


    if ( type == DT_FLOAT )
    {
        m_properties["size"] = static_cast<int>( size * sizeof( float ) );

        m_properties["min"] = -1.0;
        m_properties["max"] = 1.0;
    }
    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();
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


    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int type = getProperty( "datatype" ).toInt();

    if ( type == DT_UNSIGNED_CHAR )
    {
        glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data );
    }

    if ( type == DT_SIGNED_SHORT )
    {
        short* data = reinterpret_cast< short* >( m_b0Data );

        int mult = 65535 / m_properties["max"].toFloat();
        short* tmpData = new short[nx*ny*nz];
        for ( int i = 0; i < nx*ny*nz; ++i )
        {
            tmpData[i] = data[i] * mult;
        }

        glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_SHORT, tmpData );
        delete[] tmpData;
    }
}
