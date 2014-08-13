/*
 * tuberenderer.h
 *
 * Created on: 26.02.2013
 * @author Ralph Schurade
 */

#ifndef TUBERENDERER_H_
#define TUBERENDERER_H_

#include "objectrenderer.h"

#include "../../algos/fib.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QColor>

class FiberSelector;
class PropertyGroup;

class TubeRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    TubeRenderer( FiberSelector* selector, std::vector<Fib>* data );
    virtual ~TubeRenderer();

    void init();
    void setExtraData( unsigned int dataFieldId );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void setupTextures();
    void setShaderVars( PropertyGroup& props );

    void initGeometry();

    void initIndexBuffer( int lod );

    void updateExtraData( unsigned int dataFieldId );
private:
    FiberSelector* m_selector;
    GLuint *vboIds;

    std::vector<Fib>* m_fibs;
    std::vector< std::vector<float> >* m_indexData;

    int m_numLines;
    int m_numPoints;


    std::vector<int>m_pointsPerLine;
    std::vector<int>m_startIndexes;

    bool m_isInitialized;
    bool m_updateExtraData;
    unsigned int m_selectedExtraData;


public slots:
    void colorChanged();

private slots:
};

#endif /* TUBERENDERER_H_ */
