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

    QVector3D getInterpolatedVector( float inx, float iny, float inz );

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
    float m_lineWidth;

    int m_blockSize;

    int m_nx;
    int m_ny;
    int m_nz;

    float m_dx;
    float m_dy;
    float m_dz;

    float m_ax;
    float m_ay;
    float m_az;
};


#endif /* STIPPLERENDERER_H_ */
