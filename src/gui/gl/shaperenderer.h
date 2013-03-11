/*
 * shaperenderer.h
 *
 *  Created on: Mar 11, 2013
 *      Author: schurade
 */

#ifndef SHAPERENDERER_H_
#define SHAPERENDERER_H_

#include "GL/glew.h"

#include <QtGui/QMatrix4x4>

class ShapeRenderer
{
public:
    ShapeRenderer();
    virtual ~ShapeRenderer();

    void init();

    void drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, float x, float y, float z, float dx, float dy, float dz, QColor color, int pickID );
    void drawSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, float x, float y, float z, float dx, float dy, float dz, QColor color, int pickID );

private:
    GLuint *vboIds;

    void initBox();
    void initSphere();
};

#endif /* SHAPERENDERER_H_ */
