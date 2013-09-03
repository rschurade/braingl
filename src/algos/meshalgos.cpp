/*
 * meshalgos.cpp
 *
 *  Created on: Sep 3, 2013
 *      Author: schurade
 */

#include "meshalgos.h"

#include "loopsubdivision.h"

#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasetmesh.h"

#include "../data/models.h"

QList<Dataset*> MeshAlgos::loopSubdivision( Dataset* ds )
{
    LoopSubdivision loop( dynamic_cast<DatasetMesh*>( ds )->getMesh() );
    DatasetMesh* dsm = new DatasetMesh( loop.getMesh(), QDir("loop subdivision") );
    QList<Dataset*> l;
    l.push_back( dsm );
    return l;
}
