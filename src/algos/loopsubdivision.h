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
    unsigned int calcEdgeVert( unsigned int triNum, unsigned int edgeV1, unsigned int edgeV2, unsigned int V3 );
    QVector3D calcNewPosition( unsigned int vertNum );
    void insertCenterTriangle( unsigned int triNum );
    void insertCornerTriangles( unsigned int triNum );
    double getAlpha( unsigned int n );

    TriangleMesh2* m_mesh;
    TriangleMesh2* m_newMesh;

    unsigned int m_numVerts;
    unsigned int m_numTris;
};

#endif /* LOOPSUBDIVISION_H_ */
