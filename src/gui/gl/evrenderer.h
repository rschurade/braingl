/*
 * shrenderer.h
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */

#ifndef EVRENDERER_H_
#define EVRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class PropertyGroup;

class EVRenderer : public ObjectRenderer
{
public:
    EVRenderer( std::vector<QVector3D>* data, int nx, int ny, int nz, float dx, float dy, float dz );
    virtual ~EVRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();
    void addGlyph( std::vector<float> &verts, float xPos, float yPos, float zPos, QVector3D data );

private:
    int m_vertCount;

    GLuint vbo;

    std::vector<QVector3D>* m_data;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    float m_scaling;
    int m_orient;
    float m_offset;
};

#endif /* TENSORRENDEREREV_H_ */
