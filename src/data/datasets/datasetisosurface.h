/*
 * datasetisosurface.h
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */

#ifndef DATASETISOSURFACE_H_
#define DATASETISOSURFACE_H_

#include "../../thirdparty/glew/include/glew.h"

#include "../../thirdparty/nifti/nifti1_io.h"

#include "../mesh/isosurfaceincludes.h"

#include "dataset.h"

class DatasetScalar;
class MeshRenderer;
class TriangleMesh2;

class DatasetIsosurface : public Dataset
{
public:
    DatasetIsosurface( DatasetScalar* ds );
    virtual ~DatasetIsosurface();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore );
    QString getValueAsString( int x, int y, int z );

protected:
    void createTexture() {};

private:
    TriangleMesh2* generateSurface( float isoValue );
    int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );
    unsigned int getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ );
    POINT3DID calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );
    POINT3DID interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 );
    TriangleMesh2* renameVerticesAndTriangles( ID2POINT3DID &id2point3did, TRIANGLEVECTOR &tvector );

    QVector<float> m_scalarField;

    TriangleMesh2* m_mesh;

    MeshRenderer* m_renderer;

    float m_oldIsoValue;

    float m_isoLevel;

    int m_nX;
    int m_nY;
    int m_nZ;

    float m_dX;
    float m_dY;
    float m_dZ;

    int m_nPointsInXDirection;
    int m_nPointsInSlice;
};

#endif /* DATASETISOSURFACE_H_ */
