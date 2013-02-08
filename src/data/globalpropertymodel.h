/*
 * globalpropertymodel.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph Schurade
 */

#ifndef GLOBALPROPERTYMODEL_H_
#define GLOBALPROPERTYMODEL_H_

#include "properties/globalpropertygroup.h"

#include <QtCore/QAbstractItemModel>

class GlobalPropertyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    GlobalPropertyModel();
    virtual ~GlobalPropertyModel();

    // reimplemented from QAbstractItemModel
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole );

private:
    GlobalPropertyGroup* m_properties;

public slots:
    void propChanged();
};

#endif /* GLOBALPROPERTYMODEL_H_ */
