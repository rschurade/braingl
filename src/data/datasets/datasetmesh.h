/*
 * datasetmesh.h
 *
 *  Created on: Jul 19, 2012
 *      Author: schurade
 */

#ifndef DATASETMESH_H_
#define DATASETMESH_H_

#include <QtCore/QString>

#include "dataset.h"

class TriangleMesh;

class DatasetMesh : public Dataset
{
public:
    DatasetMesh( TriangleMesh* mesh, QString filename = QString("new mesh") );
    virtual ~DatasetMesh();

private:
    TriangleMesh* m_mesh;
};

#endif /* DATASETMESH_H_ */
