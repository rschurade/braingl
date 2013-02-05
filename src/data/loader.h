/*
 * Loader.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "datasets/dataset.h"

#include "../thirdparty/nifti/nifti1_io.h"

#include <QtCore/QDir>
#include <QtCore/QVector>

class Loader
{
public:
    Loader( QAbstractItemModel* dataModel, QAbstractItemModel* roiModel, QAbstractItemModel* globalModel );
    virtual ~Loader();

    bool load();

    Dataset* getDataset( int id = 0 );
    int getNumDatasets();

    void setFilename( QDir fn );
    bool succes();
private:
    Fn::DatasetType determineType();

    bool loadNifti();
    bool loadNiftiHeader( QString hdrPath );

    bool loadNiftiScalar( QString fileName );
    bool loadNiftiVector3D( QString fileName );
    bool loadNiftiTensor( QString fileName );
    bool loadNiftiQBall( QString fileName );
    bool loadNiftiBingham( QString fileName );
    bool loadNiftiDWI( QString fileName );
    bool loadNiftiDWI_FNAV2( QString fileName );
    QVector<float> loadBvals( QString fileName );
    QVector<QVector3D> loadBvecs( QString fileName, QVector<float> bvals );

    bool loadFib();

    bool loadMesh();
    bool loadMeshBinary();

    QString readLine( QDataStream& in );

    QAbstractItemModel* m_dataModel;
    QAbstractItemModel* m_roiModel;
    QAbstractItemModel* m_globalModel;

    nifti_image* m_header;
    QDir m_fileName;

    Fn::DatasetType m_datasetType;
    QVector<Dataset*> m_dataset;

    bool m_success;

    template< class T > T switchByteOrder( const T value )
    {
        size_t numBytes = sizeof( T );
        T result = value;
        if( numBytes == 1 )
        {
            return result;
        }
        char *s  = reinterpret_cast< char* >( &result );
        for( size_t i = 0; i < numBytes / 2; ++i )
        {
            std::swap( s[i], s[ ( numBytes - 1 ) - i ] );
        }
        return result;
    }

    /**
     * Transform a whole array of elements (of type T and size of sizeof(T))
     * into opposite byte order.
     *
     * \param array Array containing the data
     * \param arraySize The number of elements which is not the number of
     * bytes but e.g. the number of floats
     */
    template< class T > void switchByteOrderOfArray( T *array, const size_t arraySize )
    {
        for( size_t i = 0; i < arraySize; ++i )
        {
            array[i] = switchByteOrder< T >( array[i] );
        }
    }

};

#endif /* LOADER_H_ */
