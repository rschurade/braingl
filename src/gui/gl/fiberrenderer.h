/*
 * fiberrenderer.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph
 */

#ifndef FIBERRENDERER_H_
#define FIBERRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

class FiberRenderer : public ObjectRenderer
{
public:
    FiberRenderer( QVector< QVector< float > >& data );
    virtual ~FiberRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert );

    void setRenderParams( int colorMode );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    GLuint *vboIds;

    QVector< QVector< float > > m_data;
    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    int m_colorMode;
};

#endif /* FIBERRENDERER_H_ */
