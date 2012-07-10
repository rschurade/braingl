/*
 * qball.h
 *
 *  Created on: 10.07.2012
 *      Author: Ralph
 */

#ifndef QBALL_H_
#define QBALL_H_

#include "../thirdparty/newmat10/newmat.h"

class QBall
{
public:
    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );
    static Matrix sh_base( Matrix g, int max_order );

private:
    QBall();
    virtual ~QBall();

    static double sh_base_function( int order, int degree, double theta, double phi );
};

#endif /* QBALL_H_ */
