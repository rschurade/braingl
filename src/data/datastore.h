/*
 * datastore.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include "../glew/include/glew.h"

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

    // only temporary, will be removed again
    GLuint getFirstTexture();


    /**
     * reimplemented from QAbstractItemModel
     */
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	QVariant data( const QModelIndex &index, int role ) const;
	QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	QModelIndex parent ( const QModelIndex & index ) const;

    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );
public slots:
	void moveItemUp( int row );
	void moveItemDown( int row );
	void deleteItem( int row );

private:
	QVariant datasetInfo( const QModelIndex &index ) const;
	QString getNiftiDataType( const int type ) const;

    QList< Dataset* >m_datasetList;

signals:
    void datasetListChanged();
};

#endif /* DATASTORE_H_ */
