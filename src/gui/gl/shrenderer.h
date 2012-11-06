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

class TriangleMesh;

class SHRenderer : public ObjectRenderer
{
public:
    SHRenderer( QVector<ColumnVector>* data, int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~SHRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix );

    void setRenderParams( float scaling, int orient, float offset, int lodAdjust, bool minMaxScaling );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
    int m_tris1;

    GLuint *vboIds;

    QVector<ColumnVector>* m_data;

    QVector<TriangleMesh*> m_spheres;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    float m_scaling;
    int m_orient;
    float m_offset;
    float m_lodAdjust;
    bool m_minMaxScaling;

};

#endif /* SHRENDERER_H_ */
