/*
 * datasetisosurface.h
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */

#ifndef DATASETISOSURFACE_H_
#define DATASETISOSURFACE_H_

#include "../../thirdparty/glew/include/glew.h"

#include "dataset.h"

#include "../mesh/isosurfaceincludes.h"

class QAbstractItemModel;
class DatasetScalar;
class MeshRenderer;
class TriangleMesh2;
class IsoSurfaceThread;

class DatasetIsosurface : public Dataset
{
public:
    DatasetIsosurface( DatasetScalar* ds );
    virtual ~DatasetIsosurface();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model );
    QString getValueAsString( int x, int y, int z );

protected:
    void createTexture() {};

private:
    void generateSurface();
    void renameVerticesAndTriangles();

    QVector<float> m_scalarField;

    TriangleMesh2* m_mesh;

    MeshRenderer* m_renderer;

    float m_oldIsoValue;

    float m_isoLevel;
    // List of POINT3Ds which form the isosurface.
    ID2POINT3DID m_i2pt3idVertices;

    // List of TRIANGLES which form the triangulation of the isosurface.
    TRIANGLEVECTOR m_trivecTriangles;


    int m_nX;
    int m_nY;
    int m_nZ;

    float m_dX;
    float m_dY;
    float m_dZ;

    int m_nPointsInXDirection;
    int m_nPointsInSlice;

    int m_numThreads;
    QVector<IsoSurfaceThread*> m_threads;
};

#endif /* DATASETISOSURFACE_H_ */
