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

#include <QColor>

class DatasetScalar;
class PropertyGroup;

class EVRenderer : public ObjectRenderer
{
public:
    EVRenderer( std::vector<QVector3D>* data );
    virtual ~EVRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void initGeometry( PropertyGroup& props );
    void setShaderVars( PropertyGroup& props );

    void addGlyph( std::vector<float> &verts, std::vector<float> &colors, float xPos, float yPos, float zPos, QVector3D data, float alpha );

private:
    int m_vertCount;

    GLuint vbo0;
    GLuint vbo1;

    std::vector<QVector3D>* m_data;

    float m_scaling;
    int m_orient;
    float m_offset;
    QColor m_color;
    int m_lineWidth;
};

#endif /* TENSORRENDEREREV_H_ */
