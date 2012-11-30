/*
 * datasetbingham.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */
#include "../datastore.h"

#include "../../gui/gl/binghamrenderer.h"

#include "datasetbingham.h"

DatasetBingham::DatasetBingham( QString filename, QVector<QVector<float> >* data, nifti_image* header ) :
    DatasetNifti( filename, FNDT_NIFTI_BINGHAM, header ),
    m_data( data ),
    m_renderer( 0 )
{
    m_properties["active"] = true;
    m_properties["colormap"] = 0;
    m_properties["interpolation"] = false;
    m_properties["alpha"] = 1.0;
    m_properties["order"] = 4;

    examineDataset();
}

DatasetBingham::~DatasetBingham()
{
}

QVector<QVector<float> >* DatasetBingham::getData()
{
    return m_data;
}

void DatasetBingham::examineDataset()
{
    int type = getProperty( "datatype" ).toInt();
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    int nz = getProperty( "nz" ).toInt();
    int dim = m_data->at( 0 ).size();
    m_properties["nt"] = dim;
    int size = nx * ny * nz * dim;

    if ( type == DT_FLOAT )
    {
        m_properties["size"] = static_cast<int>( size * sizeof(float) );

        m_properties["min"] = -1.0;
        m_properties["max"] = 1.0;
    }
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
}

void DatasetBingham::createTexture()
{
}

void DatasetBingham::flipX()
{
}

void DatasetBingham::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore )
{
    if ( m_renderer == 0 )
    {
        qDebug() << "ds bingham init renderer";
        m_renderer = new BinghamRenderer( m_data, m_properties["nx"].toInt(), m_properties["ny"].toInt(), m_properties["nz"].toInt(),
                m_properties["dx"].toFloat(), m_properties["dy"].toFloat(), m_properties["dz"].toFloat() );
        m_renderer->setModel( dataStore );
        m_renderer->init();
        qDebug() << "ds bingham init renderer done";
    }

    m_renderer->setRenderParams( m_properties["scaling"].toFloat(), m_properties["renderSlice"].toInt(), m_properties["offset"].toFloat(),
            m_properties["lod"].toInt(), m_properties["minmaxScaling"].toBool(), m_properties["order"].toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetBingham::getValueAsString( int x, int y, int z )
{
    int nx = getProperty( "nx" ).toInt();
    int ny = getProperty( "ny" ).toInt();
    QVector<float> data = m_data->at( x + y * nx + z * nx * ny );
    return QString::number( data[0] ) + ", " + QString::number( data[1] ) + ", " + QString::number( data[2] ) + ", " + QString::number( data[3] ) +
     ", " + QString::number( data[4] ) + ", " + QString::number( data[5] ) + ", " + QString::number( data[6] ) + ", " + QString::number( data[7] ) +
     ", " + QString::number( data[8] ) + ", " + QString::number( data[9] ) + ", " + QString::number( data[10] ) + ", " + QString::number( data[11] ) +
     ", " + QString::number( data[12] ) + ", " + QString::number( data[13] );
}
