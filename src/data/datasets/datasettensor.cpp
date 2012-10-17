/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "datasettensor.h"

DatasetTensor::DatasetTensor( QString filename, QVector<Matrix>* data, nifti_image* header ) :
        DatasetNifti( filename, FNDT_NIFTI_TENSOR, header ),
        m_data( data )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;
    m_properties["createdBy"] = FNALGO_TENSORFIT;
    m_properties["bValue"] = 500;
    m_properties["faThreshold"] = 0.01;
    m_properties["evThreshold"] = 0.01;
    m_properties["gamma"] = 0.1;
    m_properties["scaling"] = 0.5;

    examineDataset();

    m_properties["renderLowerX"] = 30;
    m_properties["renderLowerY"] = 60;
    m_properties["renderLowerZ"] = 30;
    m_properties["renderUpperX"] = m_properties["nx"].toInt() - 30;
    m_properties["renderUpperY"] = m_properties["ny"].toInt() - 20;
    m_properties["renderUpperZ"] = m_properties["nz"].toInt() - 30;
}

DatasetTensor::~DatasetTensor()
{
    m_data->clear();
}

void DatasetTensor::examineDataset()
{
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int size = nx * ny * nz;

    m_properties["size"] = static_cast<int>( 9 * size * sizeof( float ) );

    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();

    m_properties["lod"] = 2;
    m_properties["order"] = 0;
    m_properties["renderSlice"] = 1;
    m_properties["scaling"] = 1.0;
    m_properties["nt"] = 9;

    m_properties["renderUpperX"] = nx - 1;
    m_properties["renderUpperY"] = ny - 1;
    m_properties["renderUpperZ"] = nz - 1;

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["name"].toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, (float)m_data->at( i )(1,1) );
        max = qMax( max, (float)m_data->at( i )(1,1) );
        min = qMin( min, (float)m_data->at( i )(1,2) );
        max = qMax( max, (float)m_data->at( i )(1,2) );
        min = qMin( min, (float)m_data->at( i )(1,3) );
        max = qMax( max, (float)m_data->at( i )(1,3) );
        min = qMin( min, (float)m_data->at( i )(2,2) );
        max = qMax( max, (float)m_data->at( i )(2,2) );
        min = qMin( min, (float)m_data->at( i )(2,3) );
        max = qMax( max, (float)m_data->at( i )(2,3) );
        min = qMin( min, (float)m_data->at( i )(3,3) );
        max = qMax( max, (float)m_data->at( i )(3,3) );
    }

    m_properties["min"] = min;
    m_properties["max"] = max;
}

void DatasetTensor::createTexture()
{
}

QVector<Matrix>* DatasetTensor::getData()
{
    return m_data;
}

void DatasetTensor::flipX()
{
    int xDim = m_properties["nx"].toInt();
    int yDim = m_properties["ny"].toInt();
    int zDim = m_properties["nz"].toInt();

    QVector<Matrix>* newData = new QVector<Matrix>();

    for( int z = 0; z < zDim; ++z )
    {
        for( int y = 0; y < yDim; ++y )
        {
            for( int x = xDim -1; x >= 0; --x )
            {
                newData->push_back( m_data->at( x + y * xDim + z * xDim * yDim ) );
            }
        }
    }

    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
    m_header->qto_xyz.m[0][3] = 0.0;
    m_header->sto_xyz.m[0][3] = 0.0;

    m_data->clear();
    m_data = newData;
}
