/*
 * fmath.h
 *
 *  Created on: Jul 17, 2012
 *      Author: schurade
 */

#ifndef FMATH_H_
#define FMATH_H_

#include "../thirdparty/newmat10/newmat.h"

#include <QtCore/QVector>

#ifdef __WINDOWS__
    #define M_PI 3.14159265359
#endif

class FMath
{
public:
    static ColumnVector vlog( ColumnVector v );

    static double legendre_p( int k );

    static Matrix sh_base( Matrix g, int max_order );
    static double sh_base_function( int order, int degree, double theta, double phi );

    static ColumnVector moment_of_inertia( const ColumnVector& values, const QVector<ColumnVector>& points );
    static double iprod( const ColumnVector& v1, const ColumnVector& v2 );
    static ColumnVector cprod( const ColumnVector& v1, const ColumnVector& v2 );

    static void evd3x3( ColumnVector tensor, QVector<ColumnVector>& vecs, ColumnVector& vals );

    static ColumnVector cart2sphere( const ColumnVector& input );
    static ColumnVector sphere2cart( const ColumnVector& input );


    static ColumnVector SH_opt_max( const ColumnVector& startX, const ColumnVector& coeff );
    static double sh_eval( const ColumnVector& position, const ColumnVector& coeff );

    static Matrix RotationMatrix( const double angle, const ColumnVector& axis );

private:
    FMath();
    virtual ~FMath();

    static double pow2( double v )
    {
        return v * v;
    };
    static double pow3( double v )
    {
        return v * v * v;
    };
};

#endif /* FMATH_H_ */
