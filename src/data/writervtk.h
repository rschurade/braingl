/*
 * writervtk.h
 *
 *  Created on: Sep 13, 2013
 *      Author: schurade
 */

#ifndef WRITERVTK_H_
#define WRITERVTK_H_

#include <QString>

class Dataset;
class TriangleMesh2;

class WriterVTK
{
public:
    WriterVTK( Dataset* dataset, QString fileName, QString filter = "" );
    virtual ~WriterVTK();

    bool save();

private:
    Dataset* m_dataset;
    QString m_fileName;
    QString m_filter;

    void saveFibs( QString filename );
    void saveFibsAscii( QString filename );
    void saveMeshVTK( QString filename, TriangleMesh2* mesh );


    void addLineDelimiter( QByteArray& out );
    void addString( QByteArray& out, QString string, bool withLineDelimiter );

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

#endif /* WRITERVTK_H_ */
