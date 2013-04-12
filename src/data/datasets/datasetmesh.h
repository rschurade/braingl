/*
 * datasetmesh.h
 *
 * Created on: Jul 19, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETMESH_H_
#define DATASETMESH_H_

#include "dataset.h"

#include <QString>

class TriangleMesh2;
class MeshRenderer;

class DatasetMesh: public Dataset
{
public:
    DatasetMesh( TriangleMesh2* mesh, QString filename = QString( "new mesh" ) );
    virtual ~DatasetMesh();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );
    QString getValueAsString( int x, int y, int z );

    void mousePick( int pickId, QVector3D pos );

private:
    TriangleMesh2* m_mesh;
    MeshRenderer* m_renderer;
};

#endif /* DATASETMESH_H_ */
