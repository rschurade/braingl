/*
 * scalaralgos.cpp
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#include "datasets/datasetscalar.h"
#include "datasets/datasetisosurface.h"

#include "scalaralgos.h"

ScalarAlgos::ScalarAlgos()
{
}

ScalarAlgos::~ScalarAlgos()
{
}

QList<Dataset*> ScalarAlgos::isoSurface( DatasetScalar* ds )
{
    DatasetIsosurface* iso = new DatasetIsosurface( ds );
    QList<Dataset*> l;
    l.push_back( iso );
    return l;
}
