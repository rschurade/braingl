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
    setProperty( FNPROP_ACTIVE, true );
    setProperty( FNPROP_COLORMAP, 0 );
    setProperty( FNPROP_INTERPOLATION, false );
    setProperty( FNPROP_ALPHA, 1.0f );
    setProperty( FNPROP_RENDER_SLICE, 1 );
    setProperty( FNPROP_SCALING, 1.0f );
    setProperty( FNPROP_OFFSET, 0.0f );
    setProperty( FNPROP_ORDER, 4 );
    setProperty( FNPROP_LOD, 2 );

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
    int type = getProperty( FNPROP_DATATYPE ).toInt();
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    int nz = getProperty( FNPROP_NZ ).toInt();
    int dim = m_data->at( 0 ).size();
    setProperty( FNPROP_DIM, dim );
    int size = nx * ny * nz * dim;

    if ( type == DT_FLOAT )
    {
        setProperty( FNPROP_SIZE, static_cast<int>( size * sizeof(float) ) );
        setProperty( FNPROP_MIN, -1.0f );
        setProperty( FNPROP_MAX, 1.0f );
    }
    setProperty( FNPROP_LOWER_THRESHOLD, getProperty( FNPROP_MIN ).toFloat() );
    setProperty( FNPROP_UPPER_THRESHOLD, getProperty( FNPROP_MAX ).toFloat() );

    setProperty( FNPROP_RENDER_UPPER_X, nx - 1 );
    setProperty( FNPROP_RENDER_UPPER_Y, ny - 1 );
    setProperty( FNPROP_RENDER_UPPER_Z, nz - 1 );
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
        m_renderer = new BinghamRenderer( m_data, getProperty( FNPROP_NX ).toInt(),
                                                  getProperty( FNPROP_NY ).toInt(),
                                                  getProperty( FNPROP_NZ ).toInt(),
                                                  getProperty( FNPROP_DX ).toFloat(),
                                                  getProperty( FNPROP_DY ).toFloat(),
                                                  getProperty( FNPROP_DZ ).toFloat() );
        m_renderer->setModel( dataStore );
        m_renderer->init();
        qDebug() << "ds bingham init renderer done";
    }

    m_renderer->setRenderParams( getProperty( FNPROP_SCALING ).toFloat(),
                                 getProperty( FNPROP_RENDER_SLICE ).toInt(),
                                 getProperty( FNPROP_OFFSET ).toFloat(),
                                 getProperty( FNPROP_LOD ).toInt(),
                                 getProperty( FNPROP_MINMAX_SCALING ).toBool(),
                                 getProperty( FNPROP_ORDER ).toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetBingham::getValueAsString( int x, int y, int z )
{
    int nx = getProperty( FNPROP_NX ).toInt();
    int ny = getProperty( FNPROP_NY ).toInt();
    QVector<float> data = m_data->at( x + y * nx + z * nx * ny );
    return QString::number( data[0] ) + ", " + QString::number( data[1] ) + ", " + QString::number( data[2] ) + ", " + QString::number( data[3] ) +
     ", " + QString::number( data[4] ) + ", " + QString::number( data[5] ) + ", " + QString::number( data[6] ) + ", " + QString::number( data[7] ) +
     ", " + QString::number( data[8] );
}
