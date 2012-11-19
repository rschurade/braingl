/*
 * fmath.h
 *
 *  Created on: Jul 17, 2012
 *      Author: schurade
 */

#ifndef FMATH_H_
#define FMATH_H_

#include "../thirdparty/newmat10/newmat.h"

#ifdef __WINDOWS__
    #define M_PI 3.14159265359
#endif

class FMath
{
public:
    static ColumnVector vlog( ColumnVector v );

    static double legendre_p( int k );

private:
    FMath();
    virtual ~FMath();
};

#endif /* FMATH_H_ */
