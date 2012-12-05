/*
 * meshrenderer.h
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */

#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include "objectrenderer.h"

class TriangleMesh2;

class MeshRenderer  : public ObjectRenderer
{
public:
    MeshRenderer( TriangleMesh2* mesh );
    virtual ~MeshRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert );

    void setMesh( TriangleMesh2* mesh );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
    int m_tris;

    GLuint *vboIds;

    TriangleMesh2* m_mesh;

    bool m_dirty;
};

#endif /* MESHRENDERER_H_ */
