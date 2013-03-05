/*
 * shrenderer.h
 *
 *  Created on: 03.07.2012
 *      Author: Ralph Schurade
 */

#ifndef BINGHAMRENDERER_H_
#define BINGHAMRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtGui/QMatrix4x4>

class BinghamRenderer : public ObjectRenderer
{
public:
    BinghamRenderer( QVector<QVector<float> >* data, int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~BinghamRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix );

    void setRenderParams( float scaling, int orient, float offset, int lodAdjust, bool minMaxScaling, int order, bool render1, bool render2, bool render3 );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
    int m_tris1;

    GLuint *vboIds;

    QVector<QVector<float> >* m_data;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    float m_scaling;
    int m_orient;
    int m_offset;
    int m_lodAdjust;
    bool m_minMaxScaling;
    int m_order;
    bool m_render1;
    bool m_render2;
    bool m_render3;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

};

#endif /* BINGHAMRENDERER_H_ */
