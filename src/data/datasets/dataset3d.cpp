/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "dataset3d.h"

Dataset3D::Dataset3D( QString filename, QVector<QVector3D> data, nifti_image* header ) :
        DatasetNifti( filename, FNDT_NIFTI_VECTOR, header ),
        m_data( data )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;

    examineDataset();
}

Dataset3D::~Dataset3D()
{
    m_data.clear();
}

void Dataset3D::examineDataset()
{
    int type = getProperty( "datatype" ).toInt();
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int size = nx * ny * nz * 3;

    if ( type == DT_UNSIGNED_CHAR )
    {
        m_properties["size"] = static_cast<int>( size * sizeof( unsigned char ) );

        m_properties["min"] = 0;
        m_properties["max"] = 255;
    }


    if ( type == DT_FLOAT )
    {
        m_properties["size"] = static_cast<int>( size * sizeof( float ) );

        m_properties["min"] = 1.0;
        m_properties["max"] = -1.0;
    }
    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["name"].toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }
}

void Dataset3D::createTexture()
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
        float* data = new float[nx*ny*nz*3];

        int size = nx * ny * nz * 3;
        for ( int i = 0; i < size; ++i )
        {
            data[i] = qMax( data[i], data[i] * -1.0f ) / 255;
        }
        glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, data );
    }

    if ( type == DT_FLOAT )
    {
        int blockSize = nx*ny*nz;
        float* data = new float[blockSize*3];

        for ( int i = 0; i < blockSize; ++i )
        {
            data[i*3] = fabs( m_data[i].x() );
            data[i*3+1] = fabs( m_data[i].y() );
            data[i*3+2] = fabs( m_data[i].z() );
        }
        glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, data );
    }
}

void Dataset3D::flipX()
{
    int xDim = m_properties["nx"].toInt();
    int yDim = m_properties["ny"].toInt();
    int zDim = m_properties["nz"].toInt();

    QVector<QVector3D> newData;

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

void Dataset3D::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore )
{
}
