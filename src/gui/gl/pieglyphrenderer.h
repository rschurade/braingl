/*
 * pieglyphrenderer.h
 *
 *  Created on: Apr 29, 2013
 *      Author: boettgerj
 */

#ifndef PIEGLYPHRENDERER_H_
#define PIEGLYPHRENDERER_H_

#include "objectrenderer.h"

#include "../../data/properties/propertygroup.h"

class PieGlyphRenderer : public ObjectRenderer
{
public:
    PieGlyphRenderer();
    virtual ~PieGlyphRenderer();

    void init();

    void initGeometry( std::vector<float*>* pieArrays, std::vector<int>* numbers, int maxNodeCount );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void setShaderVars( PropertyGroup& props );

private:
    GLuint *vboIds;

    float* pies;
    std::vector<int>* m_numbers;
    int m_maxNodeCount;
    int np;
    int ao;
    int n;
    int m_pickId;
};

#endif /* PIEGLYPHRENDERER_H_ */
