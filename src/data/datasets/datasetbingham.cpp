/*
 * datasetbingham.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */
#include "datasetbingham.h"

#include "../datastore.h"

#include "../../gui/gl/binghamrenderer.h"

DatasetBingham::DatasetBingham( QString filename, QVector<QVector<float> > data, nifti_image* header ) :
    DatasetNifti( filename, FNDT_NIFTI_BINGHAM, header ),
    m_data( data ),
    m_renderer( 0 )
{
    m_properties.set( FNPROP_RENDER_SLICE, 1, 1, 3, true );
    m_properties.set( FNPROP_SCALING, 1.0f, 0.0f, 2.0f );
    m_properties.set( FNPROP_OFFSET, 0.0f, -0.5f, 0.5f, true );
    m_properties.set( FNPROP_ORDER, 4 );
    m_properties.set( FNPROP_LOD, 2, 0, 4, true );

    examineDataset();
}

DatasetBingham::~DatasetBingham()
{
    m_properties.set( FNPROP_ACTIVE, false );
    delete m_renderer;
}

QVector<QVector<float> >* DatasetBingham::getData()
{
    return &m_data;
}

void DatasetBingham::examineDataset()
{
    int type = m_properties.get( FNPROP_DATATYPE ).toInt();
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    int nz = m_properties.get( FNPROP_NZ ).toInt();
    int dim = m_data.at( 0 ).size();
    m_properties.set( FNPROP_DIM, dim );
    int size = nx * ny * nz * dim;

    if ( type == DT_FLOAT )
    {
        m_properties.set( FNPROP_SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties.set( FNPROP_MIN, -1.0f );
        m_properties.set( FNPROP_MAX, 1.0f );
    }
    m_properties.set( FNPROP_LOWER_THRESHOLD, m_properties.get( FNPROP_MIN ).toFloat() );
    m_properties.set( FNPROP_UPPER_THRESHOLD, m_properties.get( FNPROP_MAX ).toFloat() );

    m_properties.set( FNPROP_RENDER_UPPER_X, nx - 1 );
    m_properties.set( FNPROP_RENDER_UPPER_Y, ny - 1 );
    m_properties.set( FNPROP_RENDER_UPPER_Z, nz - 1 );
}

void DatasetBingham::createTexture()
{
}

void DatasetBingham::flipX()
{
}

void DatasetBingham::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model )
{
    if ( m_renderer == 0 )
    {
        qDebug() << "ds bingham init renderer";
        m_renderer = new BinghamRenderer( &m_data, m_properties.get( FNPROP_NX ).toInt(),
                                                  m_properties.get( FNPROP_NY ).toInt(),
                                                  m_properties.get( FNPROP_NZ ).toInt(),
                                                  m_properties.get( FNPROP_DX ).toFloat(),
                                                  m_properties.get( FNPROP_DY ).toFloat(),
                                                  m_properties.get( FNPROP_DZ ).toFloat() );
        m_renderer->setModel( model );
        m_renderer->init();
        qDebug() << "ds bingham init renderer done";
    }

    m_renderer->setRenderParams( m_properties.get( FNPROP_SCALING ).toFloat(),
                                 m_properties.get( FNPROP_RENDER_SLICE ).toInt(),
                                 m_properties.get( FNPROP_OFFSET ).toFloat(),
                                 m_properties.get( FNPROP_LOD ).toInt(),
                                 m_properties.get( FNPROP_MINMAX_SCALING ).toBool(),
                                 m_properties.get( FNPROP_ORDER ).toInt() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetBingham::getValueAsString( int x, int y, int z )
{
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    QVector<float> data = m_data.at( x + y * nx + z * nx * ny );
    return QString::number( data[0] ) + ", " + QString::number( data[1] ) + ", " + QString::number( data[2] ) + ", " + QString::number( data[3] ) +
     ", " + QString::number( data[4] ) + ", " + QString::number( data[5] ) + ", " + QString::number( data[6] ) + ", " + QString::number( data[7] ) +
     ", " + QString::number( data[8] );
}
