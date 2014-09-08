/*
 * shrenderer.h
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */

#ifndef SHRENDERER_H_
#define SHRENDERER_H_

#include "objectrenderer.h"

#include "../../data/properties/propertygroup.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QMatrix4x4>

class PropertyGroup;
class TriangleMesh2;
class SHRendererThread;

class SHRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    SHRenderer( std::vector<ColumnVector>* data );
    virtual ~SHRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

//    void createMesh( PropertyGroup& props );
    TriangleMesh2* getMesh();

protected:
    void setShaderVars( PropertyGroup& props );
    void setRenderParams( PropertyGroup& props );

    void initGeometry( PropertyGroup& props );
    void updateGlyphs();

private:
    int m_tris;

    GLuint *vboIds;

    std::vector<ColumnVector>* m_data;

    float m_scaling;
    int m_orient;
    int m_offset;
    int m_lod;
    bool m_minMaxScaling;
    bool m_hideNegativeLobes;
    int m_order;

    int m_oldLoD;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

    float* m_colorBufferPointer;

    bool m_dirty;
    int m_pickId;
    int m_renderMode;
    int m_colorMode;
    int m_colormap;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
    QColor m_color;

    SHRendererThread* m_updateThread;
    bool m_glyphsUpdated;
    bool m_updateRunning;


private slots:
    void updateThreadFinished( bool );
    //void slotNewMeshCreated( TriangleMesh2* mesh );
};

#endif /* SHRENDERER_H_ */
