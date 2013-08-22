/*
 * roiarea.h
 *
 * Created on: 07.05.2013
 * @author Ralph Schurade
 */

#ifndef ROIAREA_H_
#define ROIAREA_H_

#include "roi.h"

#include "mesh/isosurfaceincludes.h"

#include "../thirdparty/nifti/nifti1_io.h"

#include <QVector>

class MeshRenderer;
class TriangleMesh2;
class IsoSurfaceThread;

class ROIArea : public ROI
{
    Q_OBJECT

public:
    ROIArea( QVector<float> data, nifti_image* header );
    virtual ~ROIArea();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );

    QVector<float>* data();

private:
    void generateSurface();
    void renameVerticesAndTriangles();

    QVector<float> m_data;
    nifti_image* m_header;

    MeshRenderer* m_renderer;
    TriangleMesh2* m_mesh;

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

    float m_max;

    int m_nPointsInXDirection;
    int m_nPointsInSlice;

    int m_numThreads;
    QVector<IsoSurfaceThread*> m_threads;

private slots:
    void globalChanged();
};

#endif /* ROIAREA_H_ */
