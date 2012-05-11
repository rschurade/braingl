/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( QString filename, std::vector<float> data ) :
    DatasetNifti( filename, FNDT_NIFTI_SCALAR, data )
{
}

DatasetScalar::~DatasetScalar()
{
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

    float min = 100000;
    float max = 0;

    if ( getDatatype() == DT_UNSIGNED_CHAR )
    {
        std::vector<unsigned char>tmp( m_data.size(), 0 );
        for ( size_t i = 0; i < m_data.size(); ++i )
        {
            tmp[i] = static_cast<unsigned char>( m_data[i] );
            min = qMin( min, m_data[i] );
            max = qMax( max, m_data[i] );
        }
        glTexImage3D( GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, nx, ny, nz, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &tmp[ 0 ] );
    }

    qDebug() << "min: " << min << " max: " << max;
}
