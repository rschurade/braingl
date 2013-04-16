/*
 * loadervtk.h
 *
 *  Created on: Apr 5, 2013
 *      Author: schurade
 */

#ifndef LOADERVTK_H_
#define LOADERVTK_H_

#include <QFile>
#include <QString>
#include <QStringList>
#include <QVector>

class LoaderVTK
{
public:
    LoaderVTK( QString fn );
    virtual ~LoaderVTK();

    bool load();
    QString getStatus();
    int getPrimitiveType();
    QVector<float> getPoints();
    QVector<int>getPrimitives();
    QVector<QVector<float> >getPointData();
    QVector<float> getPointColors();
    QVector<QString>getDataNames();

    int getNumPoints();
    int getNumPrimitives();

private:
    bool exists();
    bool open();
    bool loadHeader();

    bool loadAscii();
    bool loadPointsAscii();
    bool loadPrimitivesAscii();
    bool loadPointDataAscii();
    bool loadPointColorsAscii();

    bool loadBinary();
    bool loadPointsBinary();
    bool loadPrimitivesBinary();
    bool loadPointColorsBinary();

    int searchBinaryFile( QString string );
    bool testPos( int pos, QString string );
    QString getStringFromBinaryFile( int pos );

    QString readLine( QDataStream& in );

    QString m_filename;
    QString m_status;
    int m_type; // 0 - undefined, 1 - ASCII, 2 - BINARY
    int m_primitiveType; // 0 - undefined, 1 - POLYGONS, 2 - LINES

    int m_numPoints;
    int m_numPrimitives;
    int m_primitiveSize;

    QFile* m_file;
    QStringList m_header;
    char* m_binaryFile;
    int m_binFileSize;

    bool m_hasPointData;
    bool m_hasPointColors;

    QVector<float>m_points;
    QVector<int>m_primitives;
    QVector<QVector<float> >m_pointData;
    QVector<QString>m_dataNames;
    QVector<float>m_pointColors;

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

#endif /* LOADERVTK_H_ */
