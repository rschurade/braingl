/*
 * shrenderer.h
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */

#ifndef SHRENDERER_H_
#define SHRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class DatasetDWI;
class TriangleMesh;

class SHRenderer : public ObjectRenderer
{
public:
    SHRenderer();
    virtual ~SHRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
    float m_x;
    float m_y;
    float m_z;
    float m_xb;
    float m_yb;
    float m_zb;

    int m_xOld;
    int m_yOld;
    int m_zOld;
    int m_xbOld;
    int m_ybOld;
    int m_zbOld;

    int m_lodOld;
    float m_scalingOld;
    int m_renderSliceOld;

    int m_tris1;

    GLuint *vboIds;

    DatasetDWI* m_dataset;

    QVector<TriangleMesh*> m_spheres;
    QVector<Matrix> m_bases;
};

#endif /* SHRENDERER_H_ */
