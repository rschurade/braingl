/*
 * fiberalgos.cpp
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#include "datasets/datasetfibers.h"

#include "../algos/fibers.h"

#include "fiberalgos.h"

FiberAlgos::FiberAlgos()
{
}

FiberAlgos::~FiberAlgos()
{
}

QList<Dataset*> FiberAlgos::thinOut( DatasetFibers* ds )
{
    Fibers* fa = new Fibers( ds );
    DatasetFibers* out = fa->thinOut();
    QList<Dataset*> l;
    l.push_back( out );
    return l;
}
