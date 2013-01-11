/*
 * bingham.h
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */

#ifndef BINGHAM_H_
#define BINGHAM_H_

#include <QtCore/QList>

class Dataset;
class DatasetSH;

class Bingham
{
public:
    Bingham();
    virtual ~Bingham();

    static QList<Dataset*> calc_bingham( DatasetSH* sh, const int lod, const int neighbourhood, const int num_max );
};

#endif /* BINGHAM_H_ */
