/*
 * datasetmesh.h
 *
 *  Created on: Jul 19, 2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETMESH_H_
#define DATASETMESH_H_

#include "dataset.h"

#include <QtCore/QString>

class QAbstractItemModel;
class TriangleMesh2;

class DatasetMesh: public Dataset
{
public:
    DatasetMesh( TriangleMesh2* mesh, QString filename = QString( "new mesh" ) );
    virtual ~DatasetMesh();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QAbstractItemModel* dataModel );
    QString getValueAsString( int x, int y, int z );

private:
    TriangleMesh2* m_mesh;
};

#endif /* DATASETMESH_H_ */
