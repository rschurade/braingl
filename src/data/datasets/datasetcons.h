/*
 * datasetcons.h
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#ifndef DATASETCONS_H_
#define DATASETCONS_H_

#include "datasetfibers.h"

#include "../../algos/connections.h"
#include "../../gui/gl/vectorglyphrenderer.h"

#include "dataset.h"

#include <QString>

class DatasetCons: public DatasetFibers
{
public:
    DatasetCons( QString filename );
    DatasetCons( Connections* cons );
    virtual ~DatasetCons();
    QString getValueAsString();

    Connections* getCons();
    float* getVectors();

    QString getSaveFilter();
    QString getDefaultSuffix();

    Connections* cons;

private:
    int consNumber;
    void init();
};

#endif /* DATASETCONS_H_ */
