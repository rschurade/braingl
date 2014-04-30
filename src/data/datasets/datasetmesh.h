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
Q_OBJECT

public:
    DatasetMesh( TriangleMesh2* mesh, QDir filename = QDir( "new mesh" ) );
    DatasetMesh( QDir filename, Fn::DatasetType type );
    virtual ~DatasetMesh();

    void initProperties();
    void setProperties();
    void finalizeProperties();

    void addMesh( TriangleMesh2* tm, QString displayString = "unknown mesh" );
    int getNumberOfMeshes();
    TriangleMesh2* getMesh();
    TriangleMesh2* getMesh( int id );
    TriangleMesh2* getMesh( QString target );

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );

    virtual QString getSaveFilter();
    virtual QString getDefaultSuffix();

protected:
    std::vector<TriangleMesh2*> m_mesh;
    MeshRenderer* m_renderer;
    std::vector<QString> m_displayList;
    QMatrix4x4 m_transform;

private slots:
    void paintModeChanged( QVariant mode );
    void makePermanent();
    void transformChanged( QVariant value );
    void applyTransform();
    virtual void slotCopyColors();

};

#endif /* DATASETMESH_H_ */
