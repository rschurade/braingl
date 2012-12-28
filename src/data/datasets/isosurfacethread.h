/*
 * isosurfacethread.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph
 */

#ifndef ISOSURFACETHREAD_H_
#define ISOSURFACETHREAD_H_

#include <QMutex>
#include <QThread>

#include "../mesh/isosurfaceincludes.h"

class IsoSurfaceThread : public QThread
{
public:
    IsoSurfaceThread( QVector<float>* scalarField, QMutex* mutex, ID2POINT3DID* i2pt3idVertices, TRIANGLEVECTOR* trivecTriangles,
                      int nx, int ny, int nz, float dx, float dy, float dz, int id );
    virtual ~IsoSurfaceThread();

    void setIsoLevel( float isoLevel );

private:
    void run();

    int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );
    unsigned int getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ );
    POINT3DID calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );
    POINT3DID interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 );

    void getXYZ( int id, int &x, int &y, int &z );

    QVector<float>* m_scalarField;
    QMutex* m_mutex;

    // List of POINT3Ds which form the isosurface.
    ID2POINT3DID* m_i2pt3idVertices;
    // List of TRIANGLES which form the triangulation of the isosurface.
    TRIANGLEVECTOR* m_trivecTriangles;

    int m_nX;
    int m_nY;
    int m_nZ;

    float m_dX;
    float m_dY;
    float m_dZ;

    int m_id;
    float m_isoLevel;

    int m_nPointsInXDirection;
    int m_nPointsInSlice;
};

#endif /* ISOSURFACETHREAD_H_ */
