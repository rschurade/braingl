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
    int m_tris1;

    GLuint *vboIds;

    DatasetTensor* m_dataset;

    QVector<TriangleMesh*> m_spheres;
};

#endif /* SHRENDERER_H_ */
