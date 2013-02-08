/*
 * singleshrenderer.h
 *
 *  Created on: 12.07.2012
 *      Author: Ralph Schurade
 */

#ifndef SINGLESHRENDERER_H_
#define SINGLESHRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtGui/QMatrix4x4>

class ArcBall;
class DatasetDWI;

class SingleSHRenderer : public ObjectRenderer
{
public:
    SingleSHRenderer( QAbstractItemModel* globalModel );
    virtual ~SingleSHRenderer();

    void resizeGL( int width, int height );

    void init();

    void initGL();
    void draw();

    void leftMouseDown( int x, int y );
    void leftMouseDrag( int x, int y );

private:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    void calcMVPMatrix();

    QAbstractItemModel* m_globalModel;

    GLuint *vboIds;

    int m_width;
    int m_height;
    float m_ratio;

    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_lastRot;
    QMatrix4x4 m_thisRot;

    float m_x;
    float m_y;
    float m_z;
    float m_xb;
    float m_yb;
    float m_zb;

    int m_tris1;

    ArcBall* m_arcBall;

    DatasetDWI* m_dataset;
};

#endif /* SINGLESHRENDERER_H_ */
