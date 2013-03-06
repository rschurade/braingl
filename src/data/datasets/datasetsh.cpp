/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: Ralph Schurade
 */
#include "datasetsh.h"

#include "../../gui/gl/shrenderer.h"

DatasetSH::DatasetSH( QString filename, QVector<ColumnVector> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_SH, header ), m_data( data ), m_renderer( 0 )
{
    m_properties.set( Fn::Property::OFFSET, 0, -1, 1, true );
    //m_properties.set( Fn::Property::SCALING, 1.0f, 0.0f, 2.0f, true );
    m_properties.set( Fn::Property::MINMAX_SCALING, false, true );
    m_properties.set( Fn::Property::HIDE_NEGATIVE_LOBES, false, true );
    m_properties.set( Fn::Property::LOD, 0, 0, 5., true );
    m_properties.set( Fn::Property::RENDER_SAGITTAL, false, true );
    m_properties.set( Fn::Property::RENDER_CORONAL, false, true );
    m_properties.set( Fn::Property::RENDER_AXIAL, true, true );

    examineDataset();
}

DatasetSH::~DatasetSH()
{
    m_data.clear();
}

void DatasetSH::examineDataset()
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();
    int size = nx * ny * nz;
    int dim = m_data.at( 0 ).Nrows();

    m_properties.set( Fn::Property::SIZE, static_cast<int>( dim * size * sizeof(float) ) );

    m_properties.set( Fn::Property::LOWER_THRESHOLD, m_properties.get( Fn::Property::MIN ).toFloat() );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, m_properties.get( Fn::Property::MAX ).toFloat() );

    m_properties.set( Fn::Property::LOD, 2 );
    m_properties.set( Fn::Property::ORDER, 0 );
    m_properties.set( Fn::Property::RENDER_SLICE, 1 );
    m_properties.set( Fn::Property::SCALING, 1.0f );
    m_properties.set( Fn::Property::DIM, dim );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties.get( Fn::Property::NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
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

    m_properties.set( Fn::Property::MIN, min );
    m_properties.set( Fn::Property::MAX, max );
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
    int xDim = m_properties.get( Fn::Property::NX ).toInt();
    int yDim = m_properties.get( Fn::Property::NY ).toInt();
    int zDim = m_properties.get( Fn::Property::NZ ).toInt();

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

void DatasetSH::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QAbstractItemModel* dataModel )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new SHRenderer( &m_data, m_properties.get( Fn::Property::NX ).toInt(),
                                              m_properties.get( Fn::Property::NY ).toInt(),
                                              m_properties.get( Fn::Property::NZ ).toInt(),
                                              m_properties.get( Fn::Property::DX ).toFloat(),
                                              m_properties.get( Fn::Property::DY ).toFloat(),
                                              m_properties.get( Fn::Property::DZ ).toFloat() );
        m_renderer->setModel( globalModel );
        m_renderer->init();
    }

    m_renderer->setRenderParams( m_properties.get( Fn::Property::SCALING ).toFloat(),
                                 m_properties.get( Fn::Property::OFFSET ).toFloat(),
                                 m_properties.get( Fn::Property::LOD ).toInt(),
                                 m_properties.get( Fn::Property::MINMAX_SCALING ).toBool(),
                                 m_properties.get( Fn::Property::HIDE_NEGATIVE_LOBES ).toBool(),
                                 m_properties.get( Fn::Property::ORDER ).toInt(),
                                 m_properties.get( Fn::Property::RENDER_SAGITTAL ).toBool(),
                                 m_properties.get( Fn::Property::RENDER_CORONAL ).toBool(),
                                 m_properties.get( Fn::Property::RENDER_AXIAL ).toBool());

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetSH::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
