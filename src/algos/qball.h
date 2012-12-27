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

class DatasetDWI;

class QBall
{
public:
    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );
    static QVector<ColumnVector>* sharpQBall( DatasetDWI* ds, int order );

private:
    QBall();
    virtual ~QBall();
};

#endif /* QBALL_H_ */
