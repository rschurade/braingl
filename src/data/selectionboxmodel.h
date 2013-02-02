/*
 * selectionboxmodel.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#ifndef SELECTIONBOXMODEL_H_
#define SELECTIONBOXMODEL_H_

#include <QtCore/QAbstractItemModel>

class SelectionBoxModel : public QAbstractItemModel
{
public:
    SelectionBoxModel( QAbstractItemModel* globalProps );
    virtual ~SelectionBoxModel();

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

private:

};

#endif /* SELECTIONBOXMODEL_H_ */
