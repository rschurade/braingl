/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "../datastore.h"
#include "../../gui/gl/shrenderer.h"

#include "datasetsh.h"

DatasetSH::DatasetSH( QString filename, QVector<ColumnVector>* data, nifti_image* header ) :
        DatasetNifti( filename, FNDT_NIFTI_SH, header ), m_data( data ), m_renderer( 0 )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;
    m_properties["createdBy"] = FNALGO_QBALL;
    m_properties["offset"] = 0.0;
    m_properties["scaling"] = 1.0;

    examineDataset();
}

DatasetSH::~DatasetSH()
{
    m_data->clear();
}

void DatasetSH::examineDataset()
{
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int size = nx * ny * nz;
    int dim = m_data->at( 0 ).Nrows();

    m_properties["size"] = static_cast<int>( dim * size * sizeof(float) );

    m_properties["lowerThreshold"] = m_properties["min"].toFloat();
    m_properties["upperThreshold"] = m_properties["max"].toFloat();

    m_properties["lod"] = 2;
    m_properties["order"] = 0;
    m_properties["renderSlice"] = 1;
    m_properties["scaling"] = 1.0;
    m_properties["nt"] = dim;

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
        ColumnVector d = m_data->at( i );
        for ( int j = 1; j < dim + 1; ++j )
        {
            min = qMin( min, (float) d( j ) );
            max = qMax( max, (float) d( j ) );
        }
    }

    m_properties["min"] = min;
    m_properties["max"] = max;
}

void DatasetSH::createTexture()
{
}

QVector<ColumnVector>* DatasetSH::getData()
{
    return m_data;
}

void DatasetSH::flipX()
{
    int xDim = m_properties["nx"].toInt();
    int yDim = m_properties["ny"].toInt();
    int zDim = m_properties["nz"].toInt();

    QVector<ColumnVector>* newData = new QVector<ColumnVector>();

    for ( int z = 0; z < zDim; ++z )
    {
        for ( int y = 0; y < yDim; ++y )
        {
            for ( int x = xDim - 1; x >= 0; --x )
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

void DatasetSH::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( m_data, m_properties["nx"].toInt(), m_properties["ny"].toInt(), m_properties["nz"].toInt(),
                m_properties["dx"].toFloat(), m_properties["dy"].toFloat(), m_properties["dz"].toFloat() );
        m_renderer->setModel( dataStore );
        m_renderer->init();
    }

    m_renderer->setRenderParams( m_properties["scaling"].toFloat(), m_properties["renderSlice"].toInt(), m_properties["offset"].toFloat(),
            m_properties["lod"].toInt(), m_properties["minmaxScaling"].toBool(), m_properties["order"].toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetSH::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
