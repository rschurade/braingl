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
    DatasetSurfaceset( QDir fn = QDir( "surfaceset" ), Fn::DatasetType type = Fn::DatasetType::MESH_ASCII );
    DatasetSurfaceset( bool schnuppe, QDir fn = QDir( "surfaceset" ), Fn::DatasetType type = Fn::DatasetType::MESH_ASCII );
    virtual ~DatasetSurfaceset();

    void addMesh( TriangleMesh2* tm, QString displayString = "unknown mesh" );

    void setProperties();

    TriangleMesh2* getMesh( QString target );
    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );

    void save1Ds();
    bool load1D();

protected:
    QVector<QString> m_displayList;
};

#endif /* DATASETSURFACESET_H_ */
