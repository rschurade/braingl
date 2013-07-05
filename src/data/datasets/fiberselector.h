/*
 * fiberselector.h
 *
 * Created on: Feb 26, 2013
 * @author schurade
 */

#ifndef FIBERSELECTOR_H_
#define FIBERSELECTOR_H_

#include "../../algos/kdtree.h"

#include <QVector>
#include <QObject>
#include <QAbstractItemModel>

class FiberSelector : public QObject
{
    Q_OBJECT

public:
    FiberSelector();
    virtual ~FiberSelector();

    void init( QVector< QVector< float > >& data );

    QVector<bool>* getSelection();

private:
    QModelIndex createIndex( int branch, int pos, int column );
    void updatePresentRois();

    int m_numLines;
    int m_numPoints;

    bool m_isInitialized;

    KdTree* m_kdTree;
    QVector<float>m_kdVerts;
    QVector<int>m_reverseIndexes;
    QVector<int>m_lineStarts;
    QVector<int>m_lineLengths;

    QVector<bool>m_rootfield;
    QList<QVector<bool> >m_branchfields;
    QList<QList<QVector<bool> > >m_bitfields;

    QVector<float> m_boxMin;
    QVector<float> m_boxMax;

    float m_x;
    float m_y;
    float m_z;
    float m_dx;
    float m_dy;
    float m_dz;

private slots:
    void roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
    void roiInserted( const QModelIndex &parent, int start, int end );
    void roiDeleted( const QModelIndex &parent, int start, int end );

    void updateROI( int branch, int pos );
    void boxTest( QVector<bool>& workfield, int left, int right, int axis );
    void sphereTest( QVector<bool>& workfield );

    void updateBranch( int branch );
    void updateRoot();

signals:
    void changed();
};

#endif /* FIBERSELECTOR_H_ */
