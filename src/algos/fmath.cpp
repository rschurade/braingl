/*
 * fmath.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: schurade
 */

#include <math.h>

#include "fmath.h"

FMath::FMath() {}
FMath::~FMath() {}

ColumnVector FMath::vlog( ColumnVector v )
{
    // allocate memory of output object:
    ColumnVector tmp( v.Nrows() );

    // for each element of the vector:
    for ( int i( 1 ); i <= v.Nrows(); ++i )
    {
        if ( v( i ) <= 0.0 )
            tmp( i ) = -1.0e+17;
        else
            tmp( i ) = log( v( i ) );
    }
    return tmp;
}

double FMath::legendre_p( int k )
{
    double z = 1.0;
    double n = 1.0;

    for ( int i = 1; i <= k + 1; i += 2 ) z *= i;
    for ( int i = 2; i <= k - 2; i += 2 ) n *= i;

    if (( k / 2 ) % 2 == 0)
    {
        return -1.0 / ( 8 * M_PI ) * ( z / n );
    }
    else
    {
        return 1.0 / ( 8 * M_PI ) * ( z / n );
    }

}
