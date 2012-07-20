/*
 * isosurface.h
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#ifndef ISOSURFACE_H_
#define ISOSURFACE_H_

#include <QtCore/QVector>

#include "isosurfaceincludes.h"

class DatasetScalar;
class TrianglMesh;

class IsoSurface
{
public:
    static TriangleMesh* generateSurface( DatasetScalar* ds, float isoValue );

private:
    static int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    static unsigned int getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ );

    static POINT3DID calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo, QVector<float>* scalarField );

    static POINT3DID interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 );

    static TriangleMesh* renameVerticesAndTriangles( ID2POINT3DID &id2point3did, TRIANGLEVECTOR &tvector );

    IsoSurface();
    virtual ~IsoSurface();

    static float isoLevel;

    static int nX;
    static int nY;
    static int nZ;

    static float dX;
    static float dY;
    static float dZ;

    static int nPointsInXDirection;;
    static int nPointsInSlice;
};

#endif /* ISOSURFACE_H_ */
