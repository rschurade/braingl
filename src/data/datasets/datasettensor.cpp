/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: Ralph Schurade
 */
#include "datasettensor.h"

#include "../models.h"

#include "../../algos/fmath.h"
#include "../../gui/gl/tensorrenderer.h"
#include "../../gui/gl/tensorrendererev.h"

DatasetTensor::DatasetTensor( QString filename, QVector<Matrix> data, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_TENSOR, header ),
    m_data( data ),
    m_logData( 0 ),
    m_renderer( 0 ),
    m_rendererEV( 0 ),
    m_renderGlpyhs( false )
{
    m_properties.set( Fn::Property::FA_THRESHOLD, 0.01f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::EV_THRESHOLD, 10.0f, 0.0f, 10.f, true );
    m_properties.set( Fn::Property::GAMMA, 0.1f, 0.0f, 10.0f, true );
    m_properties.set( Fn::Property::OFFSET, 0.0f, -1.0f, 1.0f, true );
    m_properties.set( Fn::Property::SCALING, 0.5f, 0.0f, 2.0f, true );
    m_properties.set( Fn::Property::TENSOR_RENDERMODE, 0, 0, 3, true );
    m_properties.set( Fn::Property::RENDER_SAGITTAL, false, true );
    m_properties.set( Fn::Property::RENDER_CORONAL, false, true );
    m_properties.set( Fn::Property::RENDER_AXIAL, true, true );

    examineDataset();
}

DatasetTensor::DatasetTensor( QString filename, QVector<QVector<float> > data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_TENSOR, header ), m_renderer( 0 ), m_rendererEV( 0 ), m_renderGlpyhs( false )
{
    for ( int i = 0; i < data.size(); ++i )
    {
        Matrix m( 3, 3 );
        m( 1, 1 ) = data.at( i )[0];
        m( 1, 2 ) = data.at( i )[1];
        m( 1, 3 ) = data.at( i )[2];
        m( 2, 1 ) = data.at( i )[1];
        m( 2, 2 ) = data.at( i )[3];
        m( 2, 3 ) = data.at( i )[4];
        m( 3, 1 ) = data.at( i )[2];
        m( 3, 2 ) = data.at( i )[4];
        m( 3, 3 ) = data.at( i )[5];

        m_data.push_back( m );
    }

    m_properties.set( Fn::Property::CREATED_BY, (int)Fn::Algo::TENSORFIT );
    m_properties.set( Fn::Property::FA_THRESHOLD, 0.01f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::EV_THRESHOLD, 10.0f, 0.0f, 10.f, true );
    m_properties.set( Fn::Property::GAMMA, 0.1f, 0.0f, 10.0f, true );
    m_properties.set( Fn::Property::OFFSET, 0.0f, -0.5f, 0.5f, true );
    m_properties.set( Fn::Property::SCALING, 0.5f, 0.0f, 2.0f, true );
    m_properties.set( Fn::Property::TENSOR_RENDERMODE, 0, 0, 3, true );
    m_properties.set( Fn::Property::RENDER_SAGITTAL, false, true );
    m_properties.set( Fn::Property::RENDER_CORONAL, false, true );
    m_properties.set( Fn::Property::RENDER_AXIAL, true, true );

    examineDataset();
}


DatasetTensor::~DatasetTensor()
{
    m_data.clear();
}

void DatasetTensor::examineDataset()
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();
    int size = nx * ny * nz;

    m_properties.set( Fn::Property::SIZE, static_cast<int>( 9 * size * sizeof(float) ) );

    m_properties.set( Fn::Property::LOWER_THRESHOLD, m_properties.get( Fn::Property::MIN ).toFloat() );
    m_properties.set( Fn::Property::UPPER_THRESHOLD, m_properties.get( Fn::Property::MAX ).toFloat() );

    m_properties.set( Fn::Property::RENDER_SLICE, 1 );
    m_properties.set( Fn::Property::SCALING, 1.0f );
    m_properties.set( Fn::Property::DIM, 9 );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties.get( Fn::Property::NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, (float) m_data.at( i )( 1, 1 ) );
        max = qMax( max, (float) m_data.at( i )( 1, 1 ) );
        min = qMin( min, (float) m_data.at( i )( 1, 2 ) );
        max = qMax( max, (float) m_data.at( i )( 1, 2 ) );
        min = qMin( min, (float) m_data.at( i )( 1, 3 ) );
        max = qMax( max, (float) m_data.at( i )( 1, 3 ) );
        min = qMin( min, (float) m_data.at( i )( 2, 2 ) );
        max = qMax( max, (float) m_data.at( i )( 2, 2 ) );
        min = qMin( min, (float) m_data.at( i )( 2, 3 ) );
        max = qMax( max, (float) m_data.at( i )( 2, 3 ) );
        min = qMin( min, (float) m_data.at( i )( 3, 3 ) );
        max = qMax( max, (float) m_data.at( i )( 3, 3 ) );
    }

    m_properties.set( Fn::Property::MIN, min );
    m_properties.set( Fn::Property::MAX, max );
}

void DatasetTensor::createTexture()
{
}

QVector<Matrix>* DatasetTensor::getData()
{
    return &m_data;
}

QVector<Matrix>* DatasetTensor::getLogData()
{
    if ( m_logData.empty() )
    {
        createLogTensors();
    }
    return &m_logData;
}

void DatasetTensor::createLogTensors()
{
    qDebug() << "create log tensors...";

    int blockSize = m_data.size();
    m_logData.resize( blockSize );

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( m_data, evec1, eval1, evec2, eval2, evec3, eval3 );


    //log(M) =Ulog(D)UT
    Matrix U(3,3);
    DiagonalMatrix D(3);
    Matrix logM(3,3);
    for ( int i = 0; i < m_logData.size(); ++i )
    {
        U( 1, 1 ) = evec1[i].x();
        U( 2, 1 ) = evec1[i].y();
        U( 3, 1 ) = evec1[i].z();
        U( 1, 2 ) = evec2[i].x();
        U( 2, 2 ) = evec2[i].y();
        U( 3, 2 ) = evec2[i].z();
        U( 1, 3 ) = evec3[i].x();
        U( 2, 3 ) = evec3[i].y();
        U( 3, 3 ) = evec3[i].z();
        D(1) = log( eval1[i] );
        D(2) = log( eval2[i] );
        D(3) = log( eval3[i] );

        logM = U*D*U.t();

        m_logData.replace( i, logM );
    }
    qDebug() << "create log tensors done!";
}

void DatasetTensor::flipX()
{
    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();

    QVector<Matrix> newData;

    for ( int z = 0; z < nz; ++z )
    {
        for ( int y = 0; y < ny; ++y )
        {
            for ( int x = nx - 1; x >= 0; --x )
            {
                newData.push_back( m_data.at( x + y * nx + z * nx * ny ) );
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

void DatasetTensor::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
    if ( m_properties.get( Fn::Property::TENSOR_RENDERMODE ).toInt() == 0 )
    {
        if ( m_renderer == 0 )
        {
            m_renderer = new TensorRenderer( &m_data, m_properties.get( Fn::Property::NX ).toInt(),
                                                      m_properties.get( Fn::Property::NY ).toInt(),
                                                      m_properties.get( Fn::Property::NZ ).toInt(),
                                                      m_properties.get( Fn::Property::DX ).toFloat(),
                                                      m_properties.get( Fn::Property::DY ).toFloat(),
                                                      m_properties.get( Fn::Property::DZ ).toFloat() );
            m_renderer->setModel( Models::g() );
            m_renderer->init();
        }

        m_renderer->draw( pMatrix, mvMatrix, &m_properties );
    }
    else
    {
        if ( m_rendererEV == 0 )
        {
            m_rendererEV = new TensorRendererEV( &m_data, m_properties.get( Fn::Property::NX ).toInt(), m_properties.get( Fn::Property::NY ).toInt(), m_properties.get( Fn::Property::NZ ).toInt(),
                    m_properties.get( Fn::Property::DX ).toFloat(), m_properties.get( Fn::Property::DY ).toFloat(), m_properties.get( Fn::Property::DZ ).toFloat() );
            m_rendererEV->setModel( Models::g() );
            m_rendererEV->init();
        }



        m_rendererEV->draw( pMatrix, mvMatrix, &m_properties );
    }
}

QString DatasetTensor::getValueAsString( int x, int y, int z )
{
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    float m_dx = m_properties.get( Fn::Property::DX ).toFloat();
    float m_dy = m_properties.get( Fn::Property::DY ).toFloat();
    float m_dz = m_properties.get( Fn::Property::DZ ).toFloat();

    x = x * ( dx / m_dx );
    y = y * ( dy / m_dy );
    z = z * ( dz / m_dz );

    int nx = m_properties.get( Fn::Property::NX ).toInt();
    int ny = m_properties.get( Fn::Property::NY ).toInt();
    int nz = m_properties.get( Fn::Property::NZ ).toInt();

    x = qMax( 0, qMin( x, nx - 1) );
    y = qMax( 0, qMin( y, ny - 1) );
    z = qMax( 0, qMin( z, nz - 1) );

    Matrix data = m_data.at( x + y * nx + z * nx * ny );
    return QString::number( data( 1, 1 ) ) + ", " + QString::number( data( 2, 2 ) ) + ", " + QString::number( data( 3, 3 ) ) + ", "
            + QString::number( data( 1, 2 ) ) + ", " + QString::number( data( 1, 3 ) ) + ", " + QString::number( data( 2, 3 ) );
}

