/*
 * shrenderer.h
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */

#ifndef BINGHAMRENDERER_H_
#define BINGHAMRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class PropertyGroup;

class BinghamRenderer : public ObjectRenderer
{
public:
    BinghamRenderer( std::vector<std::vector<float> >* data );
    virtual ~BinghamRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );



protected:
    void initGeometry( PropertyGroup& props );
    void setShaderVars( PropertyGroup& props );
    void setRenderParams( PropertyGroup& props );

private:
    int m_tris1;

    GLuint *vboIds;

    std::vector<std::vector<float> >* m_data;

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
