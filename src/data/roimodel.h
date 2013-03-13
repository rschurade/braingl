/*
 * roimodel.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph Schurade
 */

#ifndef ROIMODEL_H_
#define ROIMODEL_H_

#include <QAbstractItemModel>

class ROI;

class ROIModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ROIModel( QAbstractItemModel* globalProps );
    virtual ~ROIModel();

    // reimplemented from QAbstractItemModel
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole );

    bool insertRows( int row, int count, const QModelIndex &parent=QModelIndex() );
    bool removeRows( int row, int count, const QModelIndex &parent=QModelIndex() );

    QModelIndexList match( const QModelIndex &start, int role, const QVariant &value, int hits=1, Qt::MatchFlags flags=Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap ) ) const;

private:
    QAbstractItemModel* m_globalProps;

    QList< QList<QVariant> >m_rois;
    int m_count;

public slots:
    void propChanged( int );

};

#endif /* ROIMODEL_H_ */
