/*
 * fiberrenderer.h
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */

#ifndef FIBERRENDERER_H_
#define FIBERRENDERER_H_

#include "objectrenderer.h"

#include "../../algos/fib.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QColor>

class FiberSelector;
class PropertyGroup;

class FiberRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    FiberRenderer( FiberSelector* selector, std::vector<Fib>* fibs, int numPoints );
    virtual ~FiberRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );
    void selectData( unsigned int dataId );

    void setExtraData( unsigned int dataFieldId );


protected:
    void setupTextures();
    void setShaderVars( PropertyGroup& props );

    void initGeometry();

    void initIndexBuffer( int lod );

    void updateExtraData( unsigned int dataFieldId );

private:
    FiberSelector* m_selector;
    GLuint vbo;
    GLuint dataVbo;
    GLuint indexVbo;

    std::vector<Fib>* m_fibs;

    unsigned int m_numLines;
    unsigned int m_numPoints;

    std::vector<unsigned int>m_pointsPerLine;
    std::vector<unsigned int>m_startIndexes;

    bool m_isInitialized;
    bool m_updateExtraData;
    unsigned int m_selectedExtraData;

public slots:
    void colorChanged();

private slots:
};

#endif /* FIBERRENDERER_H_ */
