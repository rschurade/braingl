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

class TriangleMesh2;

class DatasetMesh: public Dataset
{
public:
    DatasetMesh( TriangleMesh2* mesh, QString filename = QString( "new mesh" ) );
    virtual ~DatasetMesh();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore );
    QString getValueAsString( int x, int y, int z );

private:
    TriangleMesh2* m_mesh;
};

#endif /* DATASETMESH_H_ */
