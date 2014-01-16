/*
 * meshrenderer.h
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */

#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include "objectrenderer.h"

class TriangleMesh2;
class PropertyGroup;
class ROIPropertyGroup;

class MeshRenderer  : public ObjectRenderer
{
public:
    MeshRenderer( TriangleMesh2* mesh );
    virtual ~MeshRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props );
    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup &props );

    void setMesh( TriangleMesh2* mesh );

    void beginUpdateColor();
    void endUpdateColor();
    void updateColor( int id, float r, float g, float b, float a );

protected:
    void setupTextures();
    void setShaderVars();
    virtual void setRenderParams( PropertyGroup* props );
    void initGeometry();
    int m_colorMode;

private:
    int m_tris;

    GLuint *vboIds;
    int m_pickId;

    TriangleMesh2* m_mesh;
    float* m_colorBufferPointer;

    bool m_dirty;

    int m_renderMode;
    int m_colormap;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
    QColor m_color;

    QMatrix4x4 m_mMatrix;
};

#endif /* MESHRENDERER_H_ */
