/*
 * writer.h
 *
 * Created on: Jun 1, 2012
 * @author Ralph Schurade
 */

#ifndef WRITER_H_
#define WRITER_H_

#include "datasets/dataset.h"

#include "../thirdparty/nifti/nifti1_io.h"

#include <QString>

class Writer
{
public:
    Writer( Dataset* dataset, QString fileName );
    virtual ~Writer();

    bool save();

private:
    Dataset* m_dataset;
    QString m_fileName;

    nifti_image* createHeader( int dim );

    void setDescrip( nifti_image* hdr, QString descrip );

    void saveFibs( QString filename );

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

#endif /* WRITER_H_ */
