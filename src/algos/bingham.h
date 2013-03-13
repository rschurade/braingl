/*
 * bingham.h
 *
 *  Created on: Nov 23, 2012
 *      Author: Ralph Schurade
 */

#ifndef BINGHAM_H_
#define BINGHAM_H_

#include <QList>

class Dataset;
class DatasetBingham;
class DatasetSH;

class Bingham
{
public:
    Bingham();
    virtual ~Bingham();

    static QList<Dataset*> calc_bingham( DatasetSH* sh, const int lod, const int neighbourhood, const int num_max );

    static QList<Dataset*> bingham2Tensor( DatasetBingham* );
};

#endif /* BINGHAM_H_ */
