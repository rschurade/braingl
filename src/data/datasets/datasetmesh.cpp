/*
 * datasetmesh.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: schurade
 */

#include "../mesh/trianglemesh.h"

#include "datasetmesh.h"

DatasetMesh::DatasetMesh( TriangleMesh* mesh, QString fileName ) :
        Dataset( fileName, FNDT_MESH_BINARY ), m_mesh( mesh )
{
}

DatasetMesh::~DatasetMesh()
{
}

void DatasetMesh::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore )
{
}

QString DatasetMesh::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
