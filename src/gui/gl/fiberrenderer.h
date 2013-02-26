/*
 * fiberrenderer.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph Schurade
 */

#ifndef FIBERRENDERER_H_
#define FIBERRENDERER_H_

#include "objectrenderer.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtGui/QColor>

class FiberSelector;

class FiberRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    FiberRenderer( QAbstractItemModel* roiModel, FiberSelector* selector, QVector< QVector< float > >& data, QVector< QVector< float > >& extraData );
    virtual ~FiberRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert, QAbstractItemModel* dataModel );

    void setRenderParams( int colorMode, int colormap, float selectedMin, float selectedMax, float lowerThreshold, float upperThreshold );


protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    QModelIndex createIndex( int branch, int pos, int column );

    QAbstractItemModel* m_roiModel;
    FiberSelector* m_selector;
    GLuint *vboIds;

    QVector< QVector< float > > m_data;
    QVector< QVector< float > > m_extraData;

    int m_numLines;
    int m_numPoints;


    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    int m_colorMode;
    int m_colormap;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
    QVector<QColor>m_colorField;

public slots:
    void colorChanged( QColor color );

private slots:
};

#endif /* FIBERRENDERER_H_ */
