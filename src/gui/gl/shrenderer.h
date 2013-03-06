/*
 * shrenderer.h
 *
 *  Created on: 03.07.2012
 *      Author: Ralph Schurade
 */

#ifndef SHRENDERER_H_
#define SHRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtGui/QMatrix4x4>

class SHRenderer : public ObjectRenderer
{
public:
    SHRenderer( QVector<ColumnVector>* data, int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~SHRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix );

    void setRenderParams( float scaling, float offset, int lodAdjust, bool minMaxScaling, bool hideNegativeLobes, int order,
                            bool renderSagittal, bool renderCoronal, bool renderAxial );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
    int m_tris1;

    GLuint *vboIds;

    QVector<ColumnVector>* m_data;

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
    bool m_hideNegativeLobes;
    int m_order;

    int m_oldLoD;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

};

#endif /* SHRENDERER_H_ */
