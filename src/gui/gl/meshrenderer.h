/*
 * meshrenderer.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Ralph Schurade
 */

#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include "objectrenderer.h"

class TriangleMesh2;
class PropertyGroup;

class MeshRenderer  : public ObjectRenderer
{
public:
    MeshRenderer( TriangleMesh2* mesh );
    virtual ~MeshRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, PropertyGroup* props );

    void setMesh( TriangleMesh2* mesh );

protected:
    void setupTextures();
    void setShaderVars();
    void setRenderParams( PropertyGroup* props );
    void initGeometry();

private:
    int m_tris;

    GLuint *vboIds;

    TriangleMesh2* m_mesh;

    bool m_dirty;

    int m_colorMode;
    int m_colormap;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
};

#endif /* MESHRENDERER_H_ */
