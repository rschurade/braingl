/*
 * qball.cpp
 *
 *  Created on: 10.07.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "../data/datasets/datasetdwi.h"

#include "fmath.h"

#include "qball.h"

QBall::QBall()
{
}

QBall::~QBall()
{
}

Matrix QBall::calcQBallBase( Matrix gradients, double lambda, int maxOrder )
{
    //qDebug() << "start calculating qBall base";
    double sh_size( ( maxOrder + 1 ) * ( maxOrder + 2 ) / 2 );

    // check validity of input:
    if ( gradients.Nrows() == 0 )
        throw std::invalid_argument( "No gradient directions specified." );

    if ( gradients.Ncols() != 3 )
        throw std::invalid_argument( "Gradients have to be 3D." );

    // calculate spherical harmonics base:
    Matrix SH = FMath::sh_base( gradients, maxOrder );

    // calculate the Laplace-Beltrami and the Funk-Radon transformation:
    ColumnVector LBT( sh_size );
    ColumnVector FRT( sh_size );
    LBT = 0;
    FRT = 0;

    for ( int order = 0; order <= maxOrder; order += 2 )
    {
        double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( order, 0 );
        double lbt_val = lambda * order * order * ( order + 1 ) * ( order + 1 );

        for ( int degree( -order ); degree <= order; ++degree )
        {
            int i = order * ( order + 1 ) / 2 + degree;
            LBT( i + 1 ) = lbt_val;
            FRT( i + 1 ) = frt_val;
        }
    }

    // prepare the calculation of the pseudoinverse:
    Matrix B = SH.t() * SH;

    // update with Laplace-Beltrami operator:
    for ( int i = 0; i < sh_size; ++i )
    {
        B( i + 1, i + 1 ) += LBT( i + 1 );
    }

    Matrix out = B.i() * SH.t();

    // the Funk-Radon transformation:
    for ( int i = 0; i < B.Nrows(); ++i )
    {
        for ( int j = 0; j < out.Ncols(); ++j )
        {
            out( i + 1, j + 1 ) *= FRT( i + 1 );
        }
    }

    //qDebug() << "finished calculating qBall base";

    return out;
}

/*
 * CMRImage calc_sharp_q_ball(

 const CMRImage& data,
 const CMRImage& b_zero,
 const matrixT gradients,
 const baseT order,
 const rangeS r )
 {
 *
 */

QVector<ColumnVector>* QBall::sharpQBall( DatasetDWI* ds, int order )
{
    QVector<QVector3D> bvecs = ds->getBvecs();

    qDebug() << "order: " << order;

    Matrix gradients( bvecs.size(), 3 );
    for ( int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i + 1, 1 ) = bvecs.at( i ).x();
        gradients( i + 1, 2 ) = bvecs.at( i ).y();
        gradients( i + 1, 3 ) = bvecs.at( i ).z();
    }

    QVector<ColumnVector>* data = ds->getData();
    QVector<float> b0Data = ds->getB0Data();

    // inverse direction matrix for calculation:
    //const matrixT A( pseudoinverse (sh_base( gradients, order ) ) );
    Matrix B = FMath::sh_base( gradients, order );
    Matrix A = ( B.t() * B ).i() * B.t();

    QVector<ColumnVector>* qBallVector = new QVector<ColumnVector>();

    // for all voxels:
    for ( int i = 0; i < data->size(); ++i )
    {
        ColumnVector voxel( data->at( i ) / b0Data[i] );

        // regularize data data:
        regularize_sqball( 0.15, 0.15, voxel );
        voxel = FMath::vlog( FMath::vlog( voxel ) * ( -1 ) );
        ColumnVector coeff = A * voxel;

        for ( int k( 2 ); k <= order; k += 2 )
        {
            double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( k, 0 );
            double lbt_val = -k * ( k + 1 );

            for ( int degree = -k; degree <= k; degree++ )
            {
                int l = k * ( k + 1 ) / 2 + degree + 1;
                coeff( l ) *= frt_val * lbt_val;
            }
        }

        if ( b0Data[i] > 0.0 )
        {
            coeff( 1 ) = 1.0 / sqrt( 4. * M_PI );
        }

        qBallVector->push_back( coeff );
    }

    // done:
    return qBallVector;
}

void QBall::regularize_sqball( const double par_1, const double par_2, ColumnVector& data )
{
    for ( int i = 1; i <= data.Nrows(); ++i )
    {
        if ( data( i ) < 0 )
        {
            data( i ) = 0.5 * par_1;
        }
        else if ( data( i ) < par_1 )
        {
            data( i ) = 0.5 * par_1 + 0.5 * pow2( data( i ) ) / par_1;
        }
        else if ( data( i ) < 1.0 - par_2 )
        {
            // do nothing with data
        }
        else if ( data( i ) < 1.0 )
        {
            data( i ) = 1.0 - 0.5 * par_2 - 0.5 * pow2( 1.0 - data( i ) ) / par_2;
        }
        else
        {
            data( i ) = 1.0 - 0.5 * par_2;
        }
    }
}
