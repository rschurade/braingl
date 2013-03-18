/*
 * datasetmesh.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: Ralph Schurade
 */
#include "datasetmesh.h"

#include "../mesh/trianglemesh2.h"

DatasetMesh::DatasetMesh( TriangleMesh2* mesh, QString fileName ) :
        Dataset( fileName, Fn::DatasetType::MESH_BINARY ), m_mesh( mesh )
{
}

DatasetMesh::~DatasetMesh()
{
}

void DatasetMesh::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix )
{
}

QString DatasetMesh::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
