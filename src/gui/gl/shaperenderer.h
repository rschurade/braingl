/*
 * shaperenderer.h
 *
 * Created on: Mar 11, 2013
 * @author schurade
 */

#ifndef SHAPERENDERER_H_
#define SHAPERENDERER_H_

//#include "GL/glew.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <QMatrix4x4>

class ShapeRenderer
{
public:
    ShapeRenderer();
    virtual ~ShapeRenderer();

    void init();

    void drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                    float x, float y, float z, float dx, float dy, float dz,
                    QColor color, int pickID, int width, int height, int renderMode );

    void drawSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                        float x, float y, float z, float dx, float dy, float dz,
                        QColor color, int pickID, int width, int height, int renderMode );

private:
    GLuint *vboIds;

    void initBox();
    void initSphere();
};

#endif /* SHAPERENDERER_H_ */
