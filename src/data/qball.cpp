/*
 * qball.cpp
 *
 *  Created on: 10.07.2012
 *      Author: Ralph
 */
#include <QtCore/QDebug>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "qball.h"

QBall::QBall()
{
}

QBall::~QBall()
{
}

Matrix QBall::calcQBallBase( Matrix gradients, double lambda, int maxOrder )
{
    qDebug() << "start calculating qBall base";
    double sh_size( ( maxOrder + 1 ) * ( maxOrder + 2 ) / 2 );

    // check validity of input:
    if ( gradients.Nrows() == 0 )
        throw std::invalid_argument( "No gradient directions specified." );

    if ( gradients.Ncols() != 3 )
        throw std::invalid_argument( "Gradients have to be 3D." );

    // calculate spherical harmonics base:
    Matrix SH = sh_base( gradients, maxOrder );


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
            LBT( i+1 ) = lbt_val;
            FRT( i+1 ) = frt_val;
        }
    }

    // prepare the calculation of the pseudoinverse:
    Matrix B = SH.t() * SH;

    // update with Laplace-Beltrami operator:
    for( int i = 0; i < sh_size; ++i )
    {
        B(i+1,i+1) += LBT(i+1);
    }

    Matrix out = B.i() * SH.t();

    // the Funk-Radon transformation:
    for ( unsigned long i = 0; i < B.Nrows(); ++i )
    {
        for ( unsigned long j = 0; j < out.Ncols(); ++j )
        {
            out( i+1, j+1 ) *= FRT( i+1 );
        }
    }

    qDebug() << "finished calculating qBall base";

    return out;
}


Matrix QBall::sh_base( Matrix g, int maxOrder )
{
    qDebug() << "start calculating sh base";
  // allcoate result matrix
    unsigned long sh_dirs( ( maxOrder + 2 ) * ( maxOrder + 1 ) / 2 );
    Matrix out( g.Nrows(), sh_dirs );
    out = 0.0;

    // for each direction
    for ( int i = 0; i < g.Nrows(); ++i )
    {
        // transform current direction to polar coordinates
        double theta( acos( g( i+1, 3 ) ) );
        double phi( atan2( g( i+1, 2 ), g( i+1, 1 ) ) );

        // calculate spherical harmonic base
        for ( int order = 0, j=0; order <= maxOrder; order += 2 )
        {
            for ( int degree( -order ); degree <= order; ++degree, ++j )
            {
                out( i+1, j+1 ) = sh_base_function( order, degree, theta, phi );
            }
        }
    }
    qDebug() << "finished calculating sh base";
    return out;
}

double QBall::sh_base_function( int order, int degree, double theta, double phi )
{
    using namespace boost::math;
#if 1
    double P = legendre_p<double>( order, abs(degree), cos(theta) );

    if ( degree > 0 )
    {
        return P * cos( degree * phi );
    }
    else if ( degree < 0 )
    {
        return P * sin( -degree * phi );
    }
    else
    {
        return P;
    }
#else
    if ( degree > 0 )
    {
        return spherical_harmonic_r( order, abs( degree ), theta, phi );
    }
    else if ( degree < 0 )
    {
        return spherical_harmonic_i( order, abs( degree ), theta, phi );
    }
    else
    {
        return spherical_harmonic_r( order, 0, theta, phi );
    }
#endif
}
