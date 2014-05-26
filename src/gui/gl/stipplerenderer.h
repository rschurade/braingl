/*
 * stipplerenderer.h
 *
 *  Created on: May 26, 2014
 *      Author: schurade
 */

#ifndef STIPPLERENDERER_H_
#define STIPPLERENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QColor>

class DatasetScalar;
class PropertyGroup;

class StippleRenderer : public ObjectRenderer
{
public:
    StippleRenderer( std::vector<QVector3D>* data );
    virtual ~StippleRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

    void setMask( DatasetScalar* mask );

protected:
    void initGeometry( PropertyGroup& props );
    void setShaderVars( PropertyGroup& props );

    void addGlyph( std::vector<float> &verts, std::vector<float> &colors, float xPos, float yPos, float zPos, QVector3D data, float alpha );

private:
    int m_vertCount;

    GLuint vbo0;
    GLuint vbo1;

    std::vector<QVector3D>* m_data;
    DatasetScalar* m_mask;

    float m_scaling;
    int m_orient;
    float m_offset;
    QColor m_color;
    int m_lineWidth;
};


#endif /* STIPPLERENDERER_H_ */
