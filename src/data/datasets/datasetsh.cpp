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
    setProperty( FNPROP_ACTIVE, true );
    setProperty( FNPROP_COLORMAP, 0 );
    setProperty( FNPROP_INTERPOLATION, false );
    setProperty( FNPROP_ALPHA, 1.0f );
    setProperty( FNPROP_CREATED_BY, FNALGO_QBALL );
    setProperty( FNPROP_OFFSET, 0.0f );
    setProperty( FNPROP_SCALING, 1.0f );

    examineDataset();
}

DatasetSH::~DatasetSH()
{
    m_data->clear();
}

void DatasetSH::examineDataset()
{
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    int nz = getProperty( FNPROP_NZ ).toInt();
    int size = nx * ny * nz;
    int dim = m_data->at( 0 ).Nrows();

    setProperty( FNPROP_SIZE, static_cast<int>( dim * size * sizeof(float) ) );

    setProperty( FNPROP_LOWER_THRESHOLD, getProperty( FNPROP_MIN ).toFloat() );
    setProperty( FNPROP_UPPER_THRESHOLD, getProperty( FNPROP_MAX ).toFloat() );

    setProperty( FNPROP_LOD, 2 );
    setProperty( FNPROP_ORDER, 0 );
    setProperty( FNPROP_RENDER_SLICE, 1 );
    setProperty( FNPROP_SCALING, 1.0f );
    setProperty( FNPROP_DIM, dim );

    setProperty( FNPROP_RENDER_UPPER_X, nx - 1 );
    setProperty( FNPROP_RENDER_UPPER_Y, ny - 1 );
    setProperty( FNPROP_RENDER_UPPER_Z, nz - 1 );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << getProperty( FNPROP_NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
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

    setProperty( FNPROP_MIN, min );
    setProperty( FNPROP_MAX, max );
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
    int xDim = getProperty( FNPROP_NX ).toInt();
    int yDim = getProperty( FNPROP_NY ).toInt();
    int zDim = getProperty( FNPROP_NZ ).toInt();

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
        m_renderer = new SHRenderer( m_data, getProperty( FNPROP_NX ).toInt(), getProperty( FNPROP_NY ).toInt(), getProperty( FNPROP_NZ ).toInt(),
                getProperty( FNPROP_DX ).toFloat(), getProperty( FNPROP_DY ).toFloat(), getProperty( FNPROP_DZ ).toFloat() );
        m_renderer->setModel( dataStore );
        m_renderer->init();
    }

    m_renderer->setRenderParams( getProperty( FNPROP_SCALING ).toFloat(), getProperty( FNPROP_RENDER_SLICE ).toInt(), getProperty( FNPROP_OFFSET ).toFloat(),
            getProperty( FNPROP_LOD ).toInt(), getProperty( FNPROP_MINMAX_SCALING ).toBool(), getProperty( FNPROP_ORDER ).toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetSH::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
