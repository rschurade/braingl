/*
 * fiberalgos.cpp
 *
 *  Created on: 09.07.2012
 *      Author: Ralph Schurade
 */
#include "fiberalgos.h"

#include "fibers.h"

#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetscalar.h"

#include <QtCore/QDebug>

FiberAlgos::FiberAlgos()
{
}

FiberAlgos::~FiberAlgos()
{
}

QList<Dataset*> FiberAlgos::thinOut( Dataset* ds )
{
    Fibers* fa = new Fibers( dynamic_cast<DatasetFibers*>( ds ) );
    DatasetFibers* out = fa->thinOut();
    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> FiberAlgos::tractDensity( Dataset* ds )
{
    Fibers* fa = new Fibers( dynamic_cast<DatasetFibers*>( ds ) );
    DatasetScalar* out = fa->tractDensity();
    QList<Dataset*> l;
    l.push_back( out );
    return l;
}
