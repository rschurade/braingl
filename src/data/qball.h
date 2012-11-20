/*
 * qball.h
 *
 *  Created on: 10.07.2012
 *      Author: Ralph
 */

#ifndef QBALL_H_
#define QBALL_H_

#include "../thirdparty/newmat10/newmat.h"
#include <QtCore/QVector>
#include "datasets/datasetdwi.h"

class QBall
{
public:
    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );
    static QVector<ColumnVector>* sharpQBall( DatasetDWI* ds, int order );

    static Matrix sh_base( Matrix g, int max_order );

private:
    QBall();
    virtual ~QBall();

    static void regularize_sqball( const double par_1, const double par_2, ColumnVector& data );
    static double sh_base_function( int order, int degree, double theta, double phi );

    static double pow2( double v )
    {
        return v * v;
    };
    static double pow3( double v )
    {
        return v * v * v;
    };
};

#endif /* QBALL_H_ */
