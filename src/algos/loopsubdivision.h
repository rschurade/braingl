/*
 * loopsubdivision.h
 *
 *  Created on: Sep 3, 2013
 *      Author: schurade
 */

#ifndef LOOPSUBDIVISION_H_
#define LOOPSUBDIVISION_H_

#include <QVector3D>

class TriangleMesh2;

class LoopSubdivision
{
public:
    LoopSubdivision( TriangleMesh2* mesh );
    virtual ~LoopSubdivision();

    TriangleMesh2* getMesh() { return m_mesh; };

private:
    int calcEdgeVert( int triNum, int edgeV1, int edgeV2, int V3 );
    QVector3D calcNewPosition( int vertNum );
    void insertCenterTriangle( int triNum );
    void insertCornerTriangles( int triNum );
    double getAlpha( int n );

    TriangleMesh2* m_mesh;
    TriangleMesh2* m_newMesh;

    int m_numVerts;
    int m_numTris;
};

#endif /* LOOPSUBDIVISION_H_ */
