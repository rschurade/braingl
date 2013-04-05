/*
 * loadervtk.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: schurade
 */

#include "loadervtk.h"

#include <QDir>
#include <QTextStream>


LoaderVTK::LoaderVTK( QString fn ) :
    m_filename( fn ),
    m_status( "ok" ),
    m_type( 0 ),
    m_primitiveType( 0 ),
    m_numPoints( 0 ),
    m_numPrimitives( 0 ),
    m_primitiveSize( 0 )
{
}

LoaderVTK::~LoaderVTK()
{
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
    if ( !m_file->open( QIODevice::ReadOnly | QIODevice::Text ) )
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

bool LoaderVTK::loadBinary()
{
    return false;
}
