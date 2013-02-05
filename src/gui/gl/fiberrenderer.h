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
    Q_OBJECT

public:
    FiberRenderer( QAbstractItemModel* roiModel, QVector< QVector< float > >& data );
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
    QAbstractItemModel* m_roiModel;
    GLuint *vboIds;

    QVector< QVector< float > > m_data;
    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    int m_colorMode;

private slots:
    void roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
    void roiInserted( const QModelIndex &parent, int start, int end );
    void roiDeleted( const QModelIndex &parent, int start, int end );
};

#endif /* FIBERRENDERER_H_ */
