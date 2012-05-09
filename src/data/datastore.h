/*
 * datastore.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include <QtCore/QList>
#include <QtCore/QAbstractItemModel>

class Dataset;

class DataStore : public QAbstractItemModel
{
	Q_OBJECT

public:
    DataStore();
    virtual ~DataStore();

    bool load( QString fileName );

    void addDataset( Dataset* dataset );

	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	QVariant data( const QModelIndex &index, int role ) const;
	QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	QModelIndex parent ( const QModelIndex & index ) const;

public slots:
	void moveItemUp( int row );
	void moveItemDown( int row );

private:
    QList< Dataset* >m_datasetList;

};

#endif /* DATASTORE_H_ */
