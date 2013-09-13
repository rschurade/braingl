/*
 * datasetmeg.h
 *
 *  Created on: Aug 15, 2013
 *      Author: schurade
 */

#ifndef DATASETMEG_H_
#define DATASETMEG_H_

#include "datasetmesh.h"

class TriangleMesh2;
class MeshRenderer;

class DatasetMeshTimeSeries : public DatasetMesh
{
    Q_OBJECT

public:
    DatasetMeshTimeSeries( QDir fn = QDir( "MEG set" ), Fn::DatasetType type = Fn::DatasetType::MESH_ASCII );
    virtual ~DatasetMeshTimeSeries();

    void addMesh( TriangleMesh2* tm, QString displayString = "unknown mesh" );
    void addData( QVector<float> data );

    int getNumDataPoints();
    float getData( int id, int dataPoint );

    void setProperties();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );

private:
    QVector<QString> m_displayList;
    QVector<QVector<float> >m_data;

    float m_dataMin;
    float m_dataMax;
public slots:
    void selectFrame();
    void slotPropSet( int id );
    void slotCopyColors();

};

#endif /* DATASETMEG_H_ */
