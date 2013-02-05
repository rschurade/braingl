/*
 * datasetmesh.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: schurade
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

void DatasetMesh::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel )
{
}

QString DatasetMesh::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}
