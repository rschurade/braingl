/*
 * bingham.h
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */

#ifndef BINGHAM_H_
#define BINGHAM_H_

#include <QtCore/QList>

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

class Dataset;
class DatasetSH;

class Bingham
{
public:
    Bingham();
    virtual ~Bingham();

    static QList<Dataset*> calc_bingham( DatasetSH* sh, const int lod, const int neighbourhood, const int num_max );
    static QVector<QVector<float> > fit_bingham( const ColumnVector& sh_data, const Matrix& tess, const QVector< QSet< int > >& adj, const Matrix& base, const int neighborhood, const int num_max );


};

#endif /* BINGHAM_H_ */
