/*
 * diffpointglyphrenderer.h
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#ifndef DIFFPOINTGLYPHRENDERER_H_
#define DIFFPOINTGLYPHRENDERER_H_

#include "objectrenderer.h"

#include "../../data/properties/propertygroup.h"

class DiffPointGlyphRenderer: public ObjectRenderer
{
public:
    DiffPointGlyphRenderer();
    virtual ~DiffPointGlyphRenderer();

    void init();

    void initGeometry( float* points, int number );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props );

protected:
    void setupTextures();
    void setShaderVars(PropertyGroup* props);


private:
    GLuint *vboIds;

    float* ps;
    int np;
    int ao;
    int m_pickId;
};

#endif /* DIFFPOINTGLYPHRENDERER_H_ */