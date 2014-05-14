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

#include <QVector>

class MeshRenderer;
class TriangleMesh2;
class IsoSurfaceThread;

class ROIArea : public ROI
{
    Q_OBJECT

public:
    ROIArea( std::vector<float> data, PropertyGroup& props );
    virtual ~ROIArea();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );

    std::vector<float>* data();

private:
    void generateSurface();
    void renameVerticesAndTriangles();

    std::vector<float> m_data;

    MeshRenderer* m_renderer;
    TriangleMesh2* m_mesh;

    float m_oldIsoValue;
    float m_isoLevel;
    // List of POINT3Ds which form the isosurface.
    ID2POINT3DID m_i2pt3idVertices;

    // List of TRIANGLES which form the triangulation of the isosurface.
    TRIANGLEVECTOR m_trivecTriangles;

    float m_max;

    int m_nPointsInXDirection;
    int m_nPointsInSlice;

    int m_numThreads;
    std::vector<IsoSurfaceThread*> m_threads;

private slots:
    void globalChanged();
};

#endif /* ROIAREA_H_ */
