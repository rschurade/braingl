/*
 * pointglyphrenderer.h
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#ifndef POINTGLYPHRENDERER_H_
#define POINTGLYPHRENDERER_H_

#include "GL/glew.h"

#include "../../data/properties/propertygroup.h"

class PointGlyphRenderer
{
public:
    PointGlyphRenderer();
    virtual ~PointGlyphRenderer();

    void init();

    void initGeometry( float* points, int number );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void setShaderVars(PropertyGroup& props);

    void setRenderParams( PropertyGroup& props );

private:
    GLuint *vboIds;

    float* ps;
    int np;
    int ao;
    int m_pickId;

    int m_colorMode;
    int m_colormap;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
    QColor m_color;
};

#endif /* POINTGLYPHRENDERER_H_ */
