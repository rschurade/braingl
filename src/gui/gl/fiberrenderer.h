/*
 * fiberrenderer.h
 *
 *  Created on: 28.12.2012
 *      Author: Ralph Schurade
 */

#ifndef FIBERRENDERER_H_
#define FIBERRENDERER_H_

#include "objectrenderer.h"

#include "../../algos/kdtree.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtGui/QColor>

class FiberRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    FiberRenderer( QAbstractItemModel* roiModel, QVector< QVector< float > >& data );
    virtual ~FiberRenderer();

    void init();

    void draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert );

    void setRenderParams( int colorMode );

    QVector<bool> getSelection();

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    void initIndexBuffer( int lod );

private:
    QModelIndex createIndex( int branch, int pos, int column );

    void updatePresentRois();

    QAbstractItemModel* m_roiModel;
    GLuint *vboIds;

    QVector< QVector< float > > m_data;

    int m_numLines;
    int m_numPoints;


    QVector<int>m_pointsPerLine;
    QVector<int>m_startIndexes;

    bool m_isInitialized;

    int m_colorMode;
    QVector<QColor>m_colorField;

    KdTree* m_kdTree;
    QVector<float>m_kdVerts;
    QVector<int>m_reverseIndexes;

    QVector<bool>m_rootfield;
    QList<QVector<bool> >m_branchfields;
    QList<QList<QVector<bool> > >m_bitfields;

    QVector<float> m_boxMin;
    QVector<float> m_boxMax;

public slots:
    void colorChanged( QColor color );

private slots:
    void roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
    void roiInserted( const QModelIndex &parent, int start, int end );
    void roiDeleted( const QModelIndex &parent, int start, int end );

    void updateBox( int branch, int pos );
    void boxTest( QVector<bool>& workfield, int left, int right, int axis );

    void updateBranch( int branch );
    void updateRoot();
};

#endif /* FIBERRENDERER_H_ */
