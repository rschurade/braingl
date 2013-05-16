/*
 * vectorglyphrenderer.h
 *
 *  Created on: Apr 25, 2013
 *      Author: boettgerj
 */

#ifndef VECTORGLYPHRENDERER_H_
#define VECTORGLYPHRENDERER_H_

#include "objectrenderer.h"

#include "../../data/properties/propertygroup.h"

class VectorGlyphRenderer: public ObjectRenderer
{
public:
    VectorGlyphRenderer();
    virtual ~VectorGlyphRenderer();

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
};

#endif /* VECTORGLYPHRENDERER_H_ */
