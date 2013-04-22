/*
 * datasetsurfaceset.h
 *
 *  Created on: Apr 22, 2013
 *      Author: boettgerj
 */

#ifndef DATASETSURFACESET_H_
#define DATASETSURFACESET_H_

#include "datasetmesh.h"

class TriangleMesh2;
class MeshRenderer;

class DatasetSurfaceset: public DatasetMesh
{
public:
    DatasetSurfaceset( QString fn = "surfaceset", Fn::DatasetType type = Fn::DatasetType::MESH_ASCII );
    virtual ~DatasetSurfaceset();

    void addMesh( TriangleMesh2* tm, QString displayString = "unknown mesh" );

    void setProperty();

    TriangleMesh2* getMesh();
    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );
    void mousePick( int pickId, QVector3D pos );

private:
    QVector<QString> m_displayList;
};

#endif /* DATASETSURFACESET_H_ */
