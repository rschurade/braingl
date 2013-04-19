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
    TubeRenderer( FiberSelector* selector, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData );
    virtual ~TubeRenderer();

    void init();

    void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, PropertyGroup* props );

protected:
    void setupTextures();
    void setShaderVars( PropertyGroup* props );

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    QModelIndex createIndex( int branch, int pos, int column );

    FiberSelector* m_selector;
    GLuint *vboIds;

    QVector< QVector< float > > m_data;
    QVector< QVector< float > > m_extraData;

    int m_numLines;
    int m_numPoints;


    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    QVector<QColor>m_colorField;

public slots:
    void colorChanged( QColor color );

private slots:
};

#endif /* TUBERENDERER_H_ */
