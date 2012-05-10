/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

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

   glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_LUMINANCE, GL_FLOAT, &m_data[ 0 ] );
}
