/*
 * datasetcorrelation.h
 *
 *  Created on: Sep 12, 2013
 *      Author: schurade
 */

#ifndef DATASETCORRELATION_H_
#define DATASETCORRELATION_H_

#include "datasetmesh.h"

#include "correlationmatrix.h"

class TriangleMesh2;
class MeshRenderer;

class DatasetCorrelation : public DatasetMesh
{
public:
    DatasetCorrelation( QDir fileName, float minThreshold, float maxThreshold, Fn::DatasetType type = Fn::DatasetType::MESH_CORRELATION );
    virtual ~DatasetCorrelation();

    void setCorrelationMatrix( float** matrix );

    virtual bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );
    virtual void setPickedID( int id );

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

    virtual QString getSaveFilter();
    virtual QString getDefaultSuffix();
    void saveBinaryMatrix( QString filename );

protected:
    float m_minThreshold;
    float m_maxThreshold;

    //float** m_correlationMatrix; //square connectivity matrix

    CorrelationMatrix* m_correlations;

    QVector<int> m_picked;
    int m_prevPickedID;

};

#endif /* DATASETCORRELATION_H_ */
