/*
 * Loader.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "datasetscalar.h"

#include "loader.h"

Loader::Loader( QString filename ) :
    m_filename( filename ),
    m_datasetType( FNDT_UNKNOWN )
{
    // TODO Auto-generated constructor stub

}

Loader::~Loader()
{
    // TODO Auto-generated destructor stub
}

Dataset* Loader::load()
{
    switch ( determineType() )
    {
        case FNDT_NIFTI_SCALAR:
            loadNifti();
            break;
        case FNDT_NIFTI_VECTOR:
            break;
        case FNDT_NIFTI_4D:
            break;
        default:
            break;
    }
}

FN_DATASET_TYPE Loader::determineType()
{
    if ( m_filename.endsWith(".nii.gz") || m_filename.endsWith(".nii") )
    {
        m_header = nifti_image_read( m_filename.toStdString().c_str(), 0 );

        // if a proper nifti header is found
        if ( m_header )
        {
            if ( m_header->dim[4] == 1 )
            {
                return FNDT_NIFTI_SCALAR;
            }
            else if ( m_header->dim[4] == 3 )
            {
                return FNDT_NIFTI_VECTOR;
            }
            else if ( m_header->dim[4] > 3 )
            {
                return FNDT_NIFTI_4D;
            }
        }
    }
    return FNDT_UNKNOWN;
}

Dataset* Loader::loadNifti()
{
    nifti_image* filedata = nifti_image_read( m_filename.toStdString().c_str(), 1 );

    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    size_t dim = m_header->dim[4];

    std::vector<float> data( blockSize, 0.0 );

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t *inputData;
            inputData = reinterpret_cast<uint8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t *inputData;
            inputData = reinterpret_cast<int16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t *inputData;
            inputData = reinterpret_cast<int32_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float *inputData;
            inputData = reinterpret_cast<float*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t *inputData;
            inputData = reinterpret_cast<int8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t *inputData;
            inputData = reinterpret_cast<uint16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = static_cast<float>( inputData[j * blockSize + i] );
                }
            }
            break;
        }
    }
    DatasetScalar* dataset = new DatasetScalar( data );
    return dataset;
}
