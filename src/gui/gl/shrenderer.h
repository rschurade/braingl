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
    int m_tris1;

    GLuint *vboIds;

    DatasetDWI* m_dataset;

    QVector<TriangleMesh*> m_spheres;
};

#endif /* SHRENDERER_H_ */
