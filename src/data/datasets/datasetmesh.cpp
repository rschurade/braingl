/*
 * datasetmesh.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: schurade
 */

#include "../mesh/trianglemesh.h"

#include "datasetmesh.h"

DatasetMesh::DatasetMesh( TriangleMesh* mesh, QString fileName ) :
    Dataset( fileName, FNDT_MESH_BINARY ),
    m_mesh( mesh )
{
}

DatasetMesh::~DatasetMesh()
{
}

