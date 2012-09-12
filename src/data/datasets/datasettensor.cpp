/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "datasettensor.h"

DatasetTensor::DatasetTensor( QString filename, QVector<Matrix>* data ) :
        DatasetNifti( filename, FNDT_NIFTI_TENSOR ),
        m_data( data )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;
    m_properties["createdBy"] = FNALGO_TENSORFIT;
}

DatasetTensor::~DatasetTensor()
{
    m_data->clear();
}

void DatasetTensor::examineDataset()
{
    int type = getProperty( "datatype" ).toInt();
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int size = nx * ny * nz * 9;

    m_properties["min"] = 1.0;
    m_properties["max"] = -1.0;

    m_properties["size"] = static_cast<int>( size * sizeof( float ) );

    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();

    m_properties["lod"] = 2;
    m_properties["order"] = 0;
    m_properties["renderSlice"] = 1;
    m_properties["scaling"] = 1.0;
    m_properties["nt"] = 9;
    //m_properties["datatype"] = DT_FLOAT;
    m_properties["renderUpperX"] = nx - 1;
    m_properties["renderUpperY"] = ny - 1;
    m_properties["renderUpperZ"] = nz - 1;

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
    m_data->clear();
    m_data = newData;
}
