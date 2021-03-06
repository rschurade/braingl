/*
 * datastore.h
 *
 * Created on: May 4, 2012
 * @author Ralph Schurade
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include <QDir>
#include <QHash>
#include <QList>
#include <QAbstractItemModel>
#include <QVector3D>

class DataStore: public QAbstractItemModel
{
    Q_OBJECT

public:
    DataStore();
    virtual ~DataStore();

    // reimplemented from QAbstractItemModel
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole );
    Qt::ItemFlags flags( const QModelIndex& index ) const;

public slots:
    void moveItemUp( int row );
    void moveItemDown( int row );
    void deleteItem( int row );

    bool submit();

private:
    QVariant getDatasetProperties( const QModelIndex &index ) const;

    void updateGlobals( int row );

    QList<QVariant> m_datasetList;

signals:
};

#endif /* DATASTORE_H_ */
