/*
 * datastore.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include "properties/propertygroup.h"

#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QVector3D>

class Dataset;

class DataStore: public QAbstractItemModel
{
Q_OBJECT

public:
    DataStore();
    virtual ~DataStore();

    bool save( int index, QString fileName );

    // reimplemented from QAbstractItemModel
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole );
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool insertRow ( int row, const QModelIndex &parent=QModelIndex() );
    bool insertRows (int row, int count, const QModelIndex &parent );

public slots:
    void moveItemUp( int row );
    void moveItemDown( int row );
    void deleteItem( int row );

    void propChanged();

private:
    QVariant getDatasetProperties( const QModelIndex &index ) const;
    QVariant getGlobal( const QModelIndex &index ) const;

    void updateGlobals();

    QList<QVariant> m_datasetList;

    PropertyGroup m_globalProperties;

signals:
};

#endif /* DATASTORE_H_ */
