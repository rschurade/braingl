/*
 * shrenderer.h
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */

#ifndef TENSORRENDERER_H_
#define TENSORRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class PropertyGroup;

class TensorRenderer : public ObjectRenderer
{
public:
    TensorRenderer( QVector<Matrix>* data, int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~TensorRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, PropertyGroup* props );

protected:
    void setupTextures();
    void setShaderVars();
    void setRenderParams( PropertyGroup* props );

    void initGeometry();
    void addGlyph( std::vector<float>& verts, float xPos, float yPos, float zPos, Matrix tensor );

private:
    int m_quads;

    GLuint *vboIds;

    QVector<Matrix>* m_data;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    float m_scaling;
    float m_faThreshold;
    float m_evThreshold;
    float m_gamma;
    int m_orient;
    float m_offset;
};

#endif /* TENSORRENDERER_H_ */
