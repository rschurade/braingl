/*
 * shrenderer.h
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */

#ifndef TENSORRENDERER_H_
#define TENSORRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class DatasetTensor;
class TriangleMesh;

class TensorRenderer : public ObjectRenderer
{
public:
    TensorRenderer();
    virtual ~TensorRenderer();

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
    bool m_minmaxScalingOld;

    int m_lowerXOld;
    int m_lowerYOld;
    int m_lowerZOld;
    int m_upperXOld;
    int m_upperYOld;
    int m_upperZOld;

    int m_tris1;

    GLuint *vboIds;

    DatasetTensor* m_dataset;

    QVector<TriangleMesh*> m_spheres;
};

#endif /* SHRENDERER_H_ */
