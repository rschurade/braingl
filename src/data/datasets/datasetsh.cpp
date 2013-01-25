/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "../datastore.h"
#include "../../gui/gl/shrenderer.h"

#include "datasetsh.h"

DatasetSH::DatasetSH( QString filename, QVector<ColumnVector> data, nifti_image* header ) :
        DatasetNifti( filename, FNDT_NIFTI_SH, header ), m_data( data ), m_renderer( 0 )
{
    m_properties.set( FNPROP_OFFSET, 0.0f, -0.5f, 0.5f, true );
    m_properties.set( FNPROP_SCALING, 1.0f, 0.0f, 2.0f, true );
    m_properties.set( FNPROP_LOD, 0, -2, 2, true );

    examineDataset();
}

DatasetSH::~DatasetSH()
{
    m_data.clear();
}

void DatasetSH::examineDataset()
{
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    int nz = m_properties.get( FNPROP_NZ ).toInt();
    int size = nx * ny * nz;
    int dim = m_data.at( 0 ).Nrows();

    m_properties.set( FNPROP_SIZE, static_cast<int>( dim * size * sizeof(float) ) );

    m_properties.set( FNPROP_LOWER_THRESHOLD, m_properties.get( FNPROP_MIN ).toFloat() );
    m_properties.set( FNPROP_UPPER_THRESHOLD, m_properties.get( FNPROP_MAX ).toFloat() );

    m_properties.set( FNPROP_LOD, 2 );
    m_properties.set( FNPROP_ORDER, 0 );
    m_properties.set( FNPROP_RENDER_SLICE, 1 );
    m_properties.set( FNPROP_SCALING, 1.0f );
    m_properties.set( FNPROP_DIM, dim );

    m_properties.set( FNPROP_RENDER_UPPER_X, nx - 1 );
    m_properties.set( FNPROP_RENDER_UPPER_Y, ny - 1 );
    m_properties.set( FNPROP_RENDER_UPPER_Z, nz - 1 );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties.get( FNPROP_NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for ( int i = 0; i < size; ++i )
    {
        ColumnVector d = m_data.at( i );
        for ( int j = 1; j < dim + 1; ++j )
        {
            min = qMin( min, (float) d( j ) );
            max = qMax( max, (float) d( j ) );
        }
    }

    m_properties.set( FNPROP_MIN, min );
    m_properties.set( FNPROP_MAX, max );
}

void DatasetSH::createTexture()
{
}

QVector<ColumnVector>* DatasetSH::getData()
{
    return &m_data;
}

void DatasetSH::flipX()
{
    int xDim = m_properties.get( FNPROP_NX ).toInt();
    int yDim = m_properties.get( FNPROP_NY ).toInt();
    int zDim = m_properties.get( FNPROP_NZ ).toInt();

    QVector<ColumnVector> newData;

    for ( int z = 0; z < zDim; ++z )
    {
        for ( int y = 0; y < yDim; ++y )
        {
            for ( int x = xDim - 1; x >= 0; --x )
            {
                newData.push_back( m_data.at( x + y * xDim + z * xDim * yDim ) );
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

void DatasetSH::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( &m_data, m_properties.get( FNPROP_NX ).toInt(), m_properties.get( FNPROP_NY ).toInt(), m_properties.get( FNPROP_NZ ).toInt(),
                m_properties.get( FNPROP_DX ).toFloat(), m_properties.get( FNPROP_DY ).toFloat(), m_properties.get( FNPROP_DZ ).toFloat() );
        m_renderer->setModel( dataStore );
        m_renderer->init();
    }

    m_renderer->setRenderParams( m_properties.get( FNPROP_SCALING ).toFloat(), m_properties.get( FNPROP_RENDER_SLICE ).toInt(), m_properties.get( FNPROP_OFFSET ).toFloat(),
            m_properties.get( FNPROP_LOD ).toInt(), m_properties.get( FNPROP_MINMAX_SCALING ).toBool(), m_properties.get( FNPROP_ORDER ).toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetSH::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
