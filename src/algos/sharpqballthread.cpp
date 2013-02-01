/*
 * sharpqballthread.cpp
 *
 *  Created on: 27.12.2012
 *      Author: Ralph
 */

#include "sharpqballthread.h"

#include "fmath.h"
#include "../data/datasets/datasetdwi.h"

#include <boost/math/special_functions/spherical_harmonic.hpp>

SharpQBallThread::SharpQBallThread( DatasetDWI* ds, int order, int id ) :
    m_ds( ds ),
    m_order( order ),
    m_id( id )
{
}

SharpQBallThread::~SharpQBallThread()
{
    m_qBallVector.clear();
}

QVector<ColumnVector> SharpQBallThread::getQBallVector()
{
    return m_qBallVector;
}

void SharpQBallThread::run()
{
    QVector<QVector3D> bvecs = m_ds->getBvecs();

    Matrix gradients( bvecs.size(), 3 );
    for ( int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i + 1, 1 ) = bvecs.at( i ).x();
        gradients( i + 1, 2 ) = bvecs.at( i ).y();
        gradients( i + 1, 3 ) = bvecs.at( i ).z();
    }

    QVector<ColumnVector>* data = m_ds->getData();
    QVector<float>* b0Data = m_ds->getB0Data();

    m_qBallVector.clear();

    // inverse direction matrix for calculation:
    //const matrixT A( pseudoinverse (sh_base( gradients, order ) ) );
    Matrix B = FMath::sh_base( gradients, m_order );
    Matrix A = ( B.t() * B ).i() * B.t();

    int numThreads = QThread::idealThreadCount();

    int chunkSize = data->size() / numThreads;

    int begin = m_id * chunkSize;
    int end = m_id * chunkSize + chunkSize;

    if ( m_id == numThreads - 1 )
    {
        end = data->size();
    }

    // for all voxels:
    for ( int i = begin; i < end; ++i )
    {
        ColumnVector voxel( data->at( i ) / b0Data->at( i ) );

        // regularize data data:
        regularize_sqball( 0.15, 0.15, voxel );
        voxel = FMath::vlog( FMath::vlog( voxel ) * ( -1 ) );
        ColumnVector coeff = A * voxel;

        for ( int k( 2 ); k <= m_order; k += 2 )
        {
            double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( k, 0 );
            double lbt_val = -k * ( k + 1 );

            for ( int degree = -k; degree <= k; degree++ )
            {
                int l = k * ( k + 1 ) / 2 + degree + 1;
                coeff( l ) *= frt_val * lbt_val;
            }
        }

        if ( b0Data->at( i ) > 0.0 )
        {
            coeff( 1 ) = 1.0 / sqrt( 4. * M_PI );
        }

        m_qBallVector.push_back( coeff );
    }
}

void SharpQBallThread::regularize_sqball( const double par_1, const double par_2, ColumnVector& data )
{
    for ( int i = 1; i <= data.Nrows(); ++i )
    {
        if ( data( i ) < 0 )
        {
            data( i ) = 0.5 * par_1;
        }
        else if ( data( i ) < par_1 )
        {
            data( i ) = 0.5 * par_1 + 0.5 * FMath::pow2( data( i ) ) / par_1;
        }
        else if ( data( i ) < 1.0 - par_2 )
        {
            // do nothing with data
        }
        else if ( data( i ) < 1.0 )
        {
            data( i ) = 1.0 - 0.5 * par_2 - 0.5 * FMath::pow2( 1.0 - data( i ) ) / par_2;
        }
        else
        {
            data( i ) = 1.0 - 0.5 * par_2;
        }
    }
}
