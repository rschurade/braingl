/*
 * Loader.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include "datasetscalar.h"
#include "dataset3d.h"

#include "loader.h"

Loader::Loader( QDir fileName ) :
    m_fileName( fileName ),
    m_datasetType( FNDT_UNKNOWN ),
    m_success( false )
{
}

Loader::~Loader()
{
}

bool Loader::succes()
{
	return m_success;
}

Dataset* Loader::getDataset()
{
	return m_dataset;
}

bool Loader::load()
{
    switch ( determineType() )
    {
        case FNDT_NIFTI_SCALAR:
        {
        	void* data = loadNifti();
            DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }
        case FNDT_NIFTI_VECTOR:
        {
            void* data = loadNifti();
            Dataset3D* dataset = new Dataset3D( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }
        case FNDT_NIFTI_4D:
            break;
        default:
            break;
    }
    return false;
}

FN_DATASET_TYPE Loader::determineType()
{
    if ( m_fileName.path().endsWith(".nii.gz") || m_fileName.path().endsWith(".nii") )
    {
        m_header = nifti_image_read( m_fileName.path().toStdString().c_str(), 0 );

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

void* Loader::loadNifti()
{
    nifti_image* filedata = nifti_image_read( m_fileName.path().toStdString().c_str(), 1 );

    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    size_t dim = m_header->dim[4];

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* data = new uint8_t[ blockSize * dim ];

            uint8_t* inputData;
            inputData = reinterpret_cast<uint8_t*>( filedata->data );

            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            uint16_t* data = new uint16_t[ blockSize * dim ];

            int16_t* inputData;
            inputData = reinterpret_cast<int16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            uint32_t* data = new uint32_t[ blockSize * dim ];

            int32_t* inputData;
            inputData = reinterpret_cast<int32_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* data = new float[ blockSize * dim ];

            float* inputData;
            inputData = reinterpret_cast<float*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* data = new int8_t[ blockSize * dim ];

            int8_t* inputData;
            inputData = reinterpret_cast<int8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* data = new uint16_t[ blockSize * dim ];

            uint16_t* inputData;
            inputData = reinterpret_cast<uint16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            delete[] inputData;
            delete filedata;
            return data;
            break;
        }
    }

    return 0;
}
