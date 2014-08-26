/*
 * orientationhelperrenderer.h
 *
 *  Created on: 26.08.2014
 *      Author: Ralph
 */

#ifndef ORIENTATIONHELPERRENDERER_H_
#define ORIENTATIONHELPERRENDERER_H_

#include "objectrenderer.h"
class OrientationHelperRenderer : public ObjectRenderer
{
public:
    OrientationHelperRenderer();
    virtual ~OrientationHelperRenderer();

    void init();

    void drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode );


private:
    GLuint *vboIds;

    void initBox();
};

#endif /* ORIENTATIONHELPERRENDERER_H_ */
