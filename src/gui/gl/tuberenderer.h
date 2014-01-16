/*
 * tuberenderer.h
 *
 * Created on: 26.02.2013
 * @author Ralph Schurade
 */

#ifndef TUBERENDERER_H_
#define TUBERENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QColor>

class FiberSelector;
class PropertyGroup;

class TubeRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    TubeRenderer( FiberSelector* selector, QVector< std::vector<float> >* data, QVector< std::vector<float> >* extraData );
    virtual ~TubeRenderer();

    void init();
    void updateExtraData( QVector< std::vector<float> >* extraData );

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props );

protected:
    void setupTextures();
    void setShaderVars( PropertyGroup* props );

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    QModelIndex createIndex( int branch, int pos, int column );

    FiberSelector* m_selector;
    GLuint *vboIds;

    QVector< std::vector<float> >* m_data;
    QVector< std::vector<float> >* m_extraData;
    QVector< std::vector<float> >* m_indexData;

    int m_numLines;
    int m_numPoints;


    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    QVector<QColor>m_colorField;
    QVector<QVector3D>m_globalColorField;

public slots:
    void colorChanged( QVariant color );

private slots:
};

#endif /* TUBERENDERER_H_ */
