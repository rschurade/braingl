/*
 * loadervtk.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: schurade
 */

#include "loadervtk.h"

#include <QDir>
#include <QTextStream>
#include <QDebug>


LoaderVTK::LoaderVTK( QString fn ) :
    m_filename( fn ),
    m_status( "ok" ),
    m_type( 0 ),
    m_primitiveType( 0 ),
    m_numPoints( 0 ),
    m_numPrimitives( 0 ),
    m_primitiveSize( 0 ),
    m_binFileSize( 0 ),
    m_bufferPointer( 0 ),
    m_hasPointData( false ),
    m_hasPrimitiveData( false ),
    m_hasPointColors( false )
{
}

LoaderVTK::~LoaderVTK()
{
    m_points.clear();
    m_primitives.clear();
    m_pointData.clear();
    m_pointDataNames.clear();
    m_pointColors.clear();

    m_points.squeeze();
    m_primitives.squeeze();
    m_pointData.squeeze();
    m_pointDataNames.squeeze();
    m_pointColors.squeeze();
}

QString LoaderVTK::getStatus()
{
    return m_status;
}

int LoaderVTK::getPrimitiveType()
{
    return m_primitiveType;
}

QVector<float> LoaderVTK::getPoints()
{
    return m_points;
}

QVector<int> LoaderVTK::getPrimitives()
{
    return m_primitives;
}

QVector<QVector<float> > LoaderVTK::getPointData()
{
    return m_pointData;
}

QVector<float> LoaderVTK::getPointColors()
{
    return m_pointColors;
}


QVector<QString>LoaderVTK::getPointDataNames()
{
    return m_pointDataNames;
}

int LoaderVTK::getNumPoints()
{
    return m_numPoints;
}

int LoaderVTK::getNumPrimitives()
{
    return m_numPrimitives;
}

QString LoaderVTK::readLine( QDataStream& in )
{
    char* s = new char[1];
    QString out;
    do
    {
        in.readRawData( s, 1 );
        out += s[0];
    }
    while( s[0] != '\n' );

    return out.mid( 0, out.size() - 1 );
}

bool LoaderVTK::load()
{
    if ( !exists() )
    {
        return false;
    }
    if ( !open() )
    {
        return false;
    }
    if ( !loadHeader() )
    {
        return false;
    }

    if ( m_type == 1 )
    {
        return loadAscii();
    }
    else if ( m_type == 2 )
    {
        return loadBinary();
    }
    return false;
}

bool LoaderVTK::exists()
{
    QDir dir( m_filename );

    if ( !dir.exists( dir.absolutePath() ) )
    {
        m_status = "*ERROR* file " + m_filename + " doesn't exist";
        return false;
    }
    else
    {
        return true;
    }
}

bool LoaderVTK::open()
{
    m_file = new QFile( m_filename );
    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        m_status = "*ERROR* couldn't open file " + m_filename;
        return false;
    }
    else
    {
        return true;
    }
}

bool LoaderVTK::loadHeader()
{
    QString line;
    QStringList tokens;

    QTextStream in( m_file );
    if( !in.atEnd() )
    {
        m_header.push_back( in.readLine() );
        m_header.push_back( in.readLine() );
        m_header.push_back( in.readLine() );
        m_header.push_back( in.readLine() );
    }
    if( m_header.size() != 4 )
    {
        m_status = "*ERROR* couldn't load header of file " + m_filename;
        return false;
    }
    if ( m_header[0] != "# vtk DataFile Version 3.0" )
    {
        m_status = "*ERROR* unexpected field at header line 0 (vtk version) in file " + m_filename;
        return false;
    }

    // skip line 1

    if ( m_header[2] == "ASCII" )
    {
        m_type = 1;
    }
    else if ( m_header[2] == "BINARY" )
    {
        m_type = 2;
    }
    else
    {
        m_status = "*ERROR* unexpected field at header line 2 (data representation) in file " + m_filename;
        return false;
    }

    if ( m_header[3] != "DATASET POLYDATA" )
    {
        m_status = "*ERROR* unexpected field at header line 3 (dataset type) in file " + m_filename;
        return false;
    }
    return true;
}

bool LoaderVTK::loadAscii()
{
    if ( !loadPointsAscii() )
    {
        return false;
    }
    if ( !loadPrimitivesAscii() )
    {
        return false;
    }
    m_hasPointData = loadPointDataAscii();
    m_hasPointColors = loadPointColorsAscii();

    return true;
}

bool LoaderVTK::loadPointsAscii()
{
    QString line;
    QStringList tokens;

    m_file->seek( 0 );
    QTextStream in( m_file );

    while( !in.atEnd() )
    {
        line = in.readLine();
        if ( ( line.startsWith( "POINTS") && line.endsWith( "float") ) )
        {
            break;
        }
    }
    if ( ( line.startsWith( "POINTS") && line.endsWith( "float") ) )
    {
        tokens = line.split( " ", QString::SkipEmptyParts );
        m_numPoints = tokens[1].toInt();
    }
    else
    {
        m_status = "*ERROR* while searching for point declaration, unexpected EOF in file " + m_filename;
        return false;
    }

    while( !in.atEnd() )
    {
        line = in.readLine();
        tokens = line.split( " ", QString::SkipEmptyParts );

        for ( int i = 0; i < tokens.size(); ++i )
        {
            m_points.push_back( tokens[i].toFloat() );
        }

        if ( m_points.size() == m_numPoints * 3 )
        {
            break;
        }
    }
    if ( m_points.size() != ( m_numPoints * 3 ) )
    {
        m_status = "*ERROR* unexpected EOF while loading POINTS in file" + m_filename;
        return false;
    }

    return true;
}

bool LoaderVTK::loadPrimitivesAscii()
{
    QString line;
    QStringList tokens;
    m_file->seek( 0 );
    QTextStream in( m_file );

    while( !in.atEnd() )
    {
        line = in.readLine();
        if ( line.startsWith( "POLYGONS") || line.startsWith( "LINES") )
        {
            break;
        }
    }
    if ( line.startsWith( "POLYGONS") )
    {
        m_primitiveType = 1;
    }
    else if ( line.startsWith( "LINES") )
    {
        m_primitiveType = 2;
    }
    else
    {
        m_status = "*ERROR* while searching for primitive declaration, unexpected EOF in file " + m_filename;
        return false;
    }

    tokens = line.split( " ", QString::SkipEmptyParts );
    m_numPrimitives = tokens[1].toInt();
    m_primitiveSize = tokens[2].toInt();

    while( !in.atEnd() )
    {
        line = in.readLine();
        tokens = line.split( " ", QString::SkipEmptyParts );

        for ( int i = 0; i < tokens.size(); ++i )
        {
            m_primitives.push_back( tokens[i].toInt() );
        }

        if ( m_primitives.size() == m_primitiveSize )
        {
            break;
        }
    }
    if ( m_primitives.size() != m_primitiveSize )
    {
        m_status = "*ERROR* unexpected EOF while loading primitives in file" + m_filename;
        return false;
    }

    return true;
}

bool LoaderVTK::loadPointDataAscii()
{
    QString line;
    QStringList tokens;
    m_file->seek( 0 );
    QTextStream in( m_file );

    while( !in.atEnd() )
    {
        line = in.readLine();
        if ( line.startsWith( "POINT_DATA") )
        {
            break;
        }
    }

    while( !in.atEnd() )
    {
        line = in.readLine();
        if ( line.startsWith( "FIELD") )
        {
            break;
        }
    }

    if ( line.startsWith( "FIELD") )
    {
        tokens = line.split( " ", QString::SkipEmptyParts );
        int numFields = tokens.last().toInt();

        for( int curField = 0; curField < numFields; ++curField )
        {
            line = in.readLine();
            tokens = line.split( " ", QString::SkipEmptyParts );

            int entrySize = tokens[1].toInt();
            int numEntries = tokens[2].toInt();

            if ( numEntries == m_numPoints )
            {
                m_pointDataNames.push_back( tokens[0] );

                QVector<float> data;
                while( !in.atEnd() )
                {
                    line = in.readLine();
                    tokens = line.split( " ", QString::SkipEmptyParts );

                    for ( int i = 0; i < tokens.size(); ++i )
                    {
                        data.push_back( tokens[i].toFloat() );
                    }

                    if ( data.size() ==  ( m_numPoints * entrySize ) )
                    {
                        break;
                    }
                }
                m_pointData.push_back( data );

            }
        }
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

bool LoaderVTK::loadPointColorsAscii()
{
    QString line;
    QStringList tokens;
    m_file->seek( 0 );
    QTextStream in( m_file );

    while( !in.atEnd() )
    {
        line = in.readLine();
        if ( line.startsWith( "POINT_DATA") )
        {
            line = in.readLine();
            if ( line.startsWith( "COLOR_SCALARS") )
            {
                break;
            }
        }
    }
    if ( !line.startsWith( "COLOR_SCALARS") )
    {
        return false;
    }

    while( !in.atEnd() )
    {
        line = in.readLine();
        tokens = line.split( " ", QString::SkipEmptyParts );

        for ( int i = 0; i < tokens.size(); ++i )
        {
            m_pointColors.push_back( tokens[i].toFloat() );
        }

        if ( m_pointColors.size() == m_numPoints * 3 )
        {
            break;
        }
    }
    if ( m_pointColors.size() != ( m_numPoints * 3 ) )
    {
        m_status = "*ERROR* unexpected EOF while loading POINT COLORS in file" + m_filename;
        return false;
    }

    return true;
}

QString LoaderVTK::readLineBinary()
{
    QString out;

    while( m_binaryFile[ m_bufferPointer ] != '\n' )
    {
        out += m_binaryFile[ m_bufferPointer++ ];
    }
    m_bufferPointer++;
    return out;
}

bool LoaderVTK::loadBinary()
{
    QDataStream in( m_file );
    m_binFileSize = m_file->size();

    m_binaryFile = new char[ m_binFileSize ];
    m_file->seek( 0 );
    //int bytesRead = in.readRawData( m_binaryFile, m_binFileSize );

    //qDebug() << "read " << bytesRead << " bytes from binary file into buffer";

    for( int i = 0; i < 4; ++i )
    {
        readLineBinary();
    }

    QString fieldHeader;
    QStringList tokens;

    bool pointsLoaded = false;
    bool primitivesLoaded = false;

    while ( m_bufferPointer < m_binFileSize )
    {
        fieldHeader = readLineBinary();
        qDebug() << fieldHeader;
        tokens = fieldHeader.split( " ", QString::SkipEmptyParts );

        if ( tokens[0] == "POINTS" )
        {
            if ( !copyPoints( tokens ) )
            {
                m_status = "*ERROR* while loading POINTS in file" + m_filename;
                return false;
            }
            pointsLoaded = true;
        }
        if ( tokens[0] == "LINES" )
        {
            m_primitiveType = 2;
            if ( !copyPrimitives( tokens ) )
            {
                m_status = "*ERROR* while loading PRIMITIVES in file" + m_filename;
                return false;
            }
            primitivesLoaded = true;
        }
        if ( tokens[0] == "POLYGONS" )
        {
            m_primitiveType = 1;
            if ( !copyPrimitives( tokens ) )
            {
                m_status = "*ERROR* while loading PRIMITIVES in file" + m_filename;
                return false;
            }
            primitivesLoaded = true;
        }
        if ( tokens[0] == "CELL_DATA" )
        {
            if ( !copyPrimitiveData( tokens ) )
            {
                if ( pointsLoaded && primitivesLoaded )
                {
                    // error in cell data but since we already have points and primitves we continue anyway
                    return true;
                }
                return false;
            }
        }
        if ( tokens[0] == "POINT_DATA" )
        {
            if ( !copyPointData( tokens ) )
            {
                // error in point data but since we already have points and primitves we continue anyway
                return true;
            }
            return false;
        }
    }

    return true;
}

bool LoaderVTK::copyPoints( QStringList tokens )
{
    if ( ( tokens.size() == 3 ) && tokens[0] == "POINTS" && tokens[2] == "float" )
    {
        m_numPoints = tokens[1].toInt();
    }
    else
    {
        m_status = "*ERROR* unexpected field in points declaration " + m_filename;
        return false;
    }
    char* pointField = new char[m_numPoints * sizeof( float ) * 3];

    int begin = m_bufferPointer;
    int end = begin + m_numPoints * sizeof( float ) * 3;
    int i, j;
    for ( i = begin, j = 0; i < end; ++i, ++j )
    {
        pointField[j] = m_binaryFile[i];
    }

    float* rawPointData = reinterpret_cast<float*>( pointField );
    switchByteOrderOfArray< float >( rawPointData, m_numPoints * 3 );

    m_points.resize( m_numPoints * 3 );
    for ( int k = 0; k < m_points.size(); ++k )
    {
        m_points[k] = rawPointData[k];
    }

    delete[] rawPointData;

    m_bufferPointer += m_numPoints * sizeof( float ) * 3 + 1;

    return true;
}

bool LoaderVTK::copyPrimitives( QStringList tokens )
{
    if ( tokens.size() == 3 )
    {
        m_numPrimitives = tokens[1].toInt();
        m_primitiveSize = tokens[2].toInt();
    }
    else
    {
        m_status = "*ERROR* unexpected field in primitive declaration " + m_filename;
        return false;
    }

    char* primitiveField = new char[ m_primitiveSize * sizeof( int ) ];

    int begin = m_bufferPointer;
    int end = begin + m_primitiveSize * sizeof( int );

    int i, j;
    for ( i = begin, j = 0; i < end; ++i, ++j )
    {
        primitiveField[j] = m_binaryFile[i];
    }

    int* rawPrimitiveData = reinterpret_cast<int*>( primitiveField );
    switchByteOrderOfArray< int >( rawPrimitiveData, m_primitiveSize );

    m_primitives.resize( m_primitiveSize );

    for ( int k = 0; k < m_primitiveSize; ++k )
    {
        m_primitives[k] = rawPrimitiveData[k];
    }

    delete[] rawPrimitiveData;

    m_bufferPointer += m_primitiveSize * sizeof( int ) + 1;

    return true;

}

bool LoaderVTK::copyPrimitiveData( QStringList tokens )
{
    if ( m_numPrimitives != tokens[1].toInt() )
    {
        m_status = "*ERROR* cell data size doesn't match num primitives " + m_filename;
    }
    QString line = readLineBinary();
    //qDebug() << line;
    QStringList toks = line.split( " ", QString::SkipEmptyParts );
    if ( toks.size() == 3 && toks[0] == "FIELD" && toks[1] == "FieldData" )
    {
        int numFields = toks[2].toInt();
        for ( int i = 0; i < numFields; ++i )
        {
            line = readLineBinary();
            //qDebug() << line;
            toks = line.split( " ", QString::SkipEmptyParts );

            if ( toks.size() == 4 && toks[1].toInt() == 1 && toks[2].toInt() == m_numPrimitives && toks[3] == "float" )
            {
                m_primitiveDataNames.push_back( toks[0] );

                char* field = new char[m_numPrimitives * sizeof( float )];

                int begin = m_bufferPointer;
                int end = begin + m_numPrimitives * sizeof( float );
                int i, j;
                for ( i = begin, j = 0; i < end; ++i, ++j )
                {
                    field[j] = m_binaryFile[i];
                }

                float* rawData = reinterpret_cast<float*>( field );
                switchByteOrderOfArray< float >( rawData, m_numPrimitives );

                QVector<float>dataField;
                dataField.resize( m_numPrimitives );

                for ( int k = 0; k < dataField.size(); ++k )
                {
                    dataField[k] = rawData[k];
                }
                delete[] rawData;
                m_primitiveData.push_back( dataField );

                m_bufferPointer += m_numPrimitives * sizeof( float ) + 1;
            }
            else
            {
                m_status = "*ERROR* unexpected field in cell data definition " + m_filename;
                return false;
            }
        }
    }
    else
    {
        m_status = "*ERROR* unexpected field in cell data fields definition " + m_filename;
        return false;
    }
    m_hasPrimitiveData = true;
    return true;
}

bool LoaderVTK::copyPointData( QStringList tokens )
{
    if ( m_numPoints != tokens[1].toInt() )
    {
        m_status != "*ERROR* point data size doesn't match num points " + m_filename;
    }
    QString line = readLineBinary();
    //qDebug() << line;
    QStringList toks = line.split( " ", QString::SkipEmptyParts );
    if ( toks.size() == 3 && toks[0] == "FIELD" && toks[1] == "FieldData" )
    {
        int numFields = toks[2].toInt();
        for ( int i = 0; i < numFields; ++i )
        {
            line = readLineBinary();
            //qDebug() << line;
            toks = line.split( " ", QString::SkipEmptyParts );

            if ( toks.size() == 4 && toks[1].toInt() == 1 && toks[2].toInt() == m_numPoints && toks[3] == "float" )
            {
                m_pointDataNames.push_back( toks[0] );

                char* field = new char[m_numPoints * sizeof( float )];

                int begin = m_bufferPointer;
                int end = begin + m_numPoints * sizeof( float );
                int i, j;
                for ( i = begin, j = 0; i < end; ++i, ++j )
                {
                    field[j] = m_binaryFile[i];
                }

                float* rawData = reinterpret_cast<float*>( field );
                switchByteOrderOfArray< float >( rawData, m_numPoints );

                QVector<float>dataField;
                dataField.resize( m_numPoints );

                for ( int k = 0; k < dataField.size(); ++k )
                {
                    dataField[k] = rawData[k];
                }
                delete[] rawData;
                m_pointData.push_back( dataField );

                m_bufferPointer += m_numPoints * sizeof( float ) + 1;
            }
            else
            {
                m_status = "*ERROR* unexpected field in point data definition " + m_filename;
                return false;
            }
            m_hasPointData = true;
        }
    }
    else if ( toks.size() == 3 && toks[0] == "NORMALS" && toks[1] == "Normals" && toks[2] == "float" )
    {
        m_bufferPointer += m_numPoints * sizeof( float ) * 3 + 1;
    }
    else if ( toks.size() == 3 && toks[0] == "COLOR_SCALARS" && toks[1] == "Colors" && toks[2].toInt() == 3 )
    {
        char* pointField = new char[m_numPoints * sizeof( float ) * 3];

        int begin = m_bufferPointer;
        int end = begin + m_numPoints * sizeof( float ) * 3;
        int i, j;
        for ( i = begin, j = 0; i < end; ++i, ++j )
        {
            pointField[j] = m_binaryFile[i];
        }

        float* rawPointData = reinterpret_cast<float*>( pointField );
        switchByteOrderOfArray< float >( rawPointData, m_numPoints * 3 );

        m_pointColors.resize( m_numPoints * 3 );
        for ( int k = 0; k < m_points.size(); ++k )
        {
            m_pointColors[k] = rawPointData[k];
        }

        delete[] rawPointData;

        m_bufferPointer += m_numPoints * sizeof( float ) * 3 + 1;
        m_hasPointColors = true;
    }
    else
    {
        m_status = "*ERROR* unexpected field in point data fields definition " + m_filename;
        return false;
    }
    return true;
}
