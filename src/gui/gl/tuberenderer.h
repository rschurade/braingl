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
#include <QObject>

class FiberSelector;
class PropertyGroup;

class TubeRenderer : public QObject
{
    Q_OBJECT

public:
    TubeRenderer( FiberSelector* selector, std::vector<Fib>* data );
    virtual ~TubeRenderer();

    void init();
    void updateExtraData( unsigned int dataFieldId );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    void setupTextures();
    void setShaderVars( PropertyGroup& props );

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    QModelIndex createIndex( int branch, int pos, int column );

    FiberSelector* m_selector;
    GLuint *vboIds;

    std::vector<Fib>* m_fibs;
    std::vector< std::vector<float> >* m_indexData;

    int m_numLines;
    int m_numPoints;


    std::vector<int>m_pointsPerLine;
    std::vector<int>m_startIndexes;

    bool m_isInitialized;

public slots:
    void colorChanged();

private slots:
};

#endif /* TUBERENDERER_H_ */
