/*
 * writervtk.cpp
 *
 *  Created on: Sep 13, 2013
 *      Author: schurade
 */

#include "writervtk.h"

#include "datasets/datasetfibers.h"
#include "datasets/datasetmesh.h"
#include "datasets/datasetmeshtimeseries.h"
#include "datasets/datasetglyphset.h"
#include "datasets/datasetisosurface.h"

#include "mesh/trianglemesh2.h"

#include <QDebug>
#include <QFile>

WriterVTK::WriterVTK( Dataset* dataset, QString fileName, QString filter ) :
    m_dataset( dataset ),
    m_fileName( fileName ),
    m_filter( filter )
{
}

WriterVTK::~WriterVTK()
{
}

bool WriterVTK::save()
{
    if ( m_fileName.endsWith( ".vtk" ) || m_fileName.endsWith( ".fib" ) )
    {
        switch ( (Fn::DatasetType)( m_dataset->properties()->get( Fn::Property::D_TYPE ).toInt() ) )
        {
            qDebug() << m_filter;
            case Fn::DatasetType::FIBERS:
            {
                if ( m_filter.startsWith( "fib files ascii") )
                {
                    saveFibsAscii( m_fileName );
                }
                else
                {
                    saveFibs( m_fileName );
                }
                break;
            }
            case Fn::DatasetType::MESH_BINARY:
            case Fn::DatasetType::MESH_ISOSURFACE:
            case Fn::DatasetType::MESH_TIME_SERIES :
            case Fn::DatasetType::GLYPHSET :
            {
                TriangleMesh2* mesh = dynamic_cast<DatasetMesh*>( m_dataset )->getMesh();
                saveMeshVTK( m_fileName, mesh );
                break;
            }
            default:
                break;
        }
    }
}

void WriterVTK::saveFibs( QString filename )
{
    QVector< QVector<float> >fibs = dynamic_cast<DatasetFibers*>( m_dataset )->getFibs();
    QVector< QVector< QVector<float> > >data = dynamic_cast<DatasetFibers*>( m_dataset )->getData();

    unsigned int numPoints = 0;
    unsigned int numLines = fibs.size();
    for( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size()/3;
    }


    unsigned int *linesOut = new unsigned int[numPoints + numLines];
    float *pointsOut = new float[numPoints * 3];

    unsigned int pntPosOffset = 0;
    unsigned int lnsPosOffset = 0;

    for( int i = 0; i < fibs.size(); ++i )
    {
        QVector<float>fib = fibs[i];
        linesOut[lnsPosOffset++] = static_cast< unsigned int >( fib.size()/3 );
        for( int j = 0; j < fib.size()/3; ++j )
        {
            linesOut[lnsPosOffset++] = static_cast< unsigned int >( pntPosOffset / 3 );
            pointsOut[pntPosOffset++] = static_cast< float >( fib[j*3] );
            pointsOut[pntPosOffset++] = static_cast< float >( fib[j*3+1] );
            pointsOut[pntPosOffset++] = static_cast< float >( fib[j*3+2] );
        }
    }

    switchByteOrderOfArray< float >( pointsOut, numPoints * 3 );
    switchByteOrderOfArray< unsigned int >( linesOut, numLines + numPoints );

    QFile file( filename );
    if ( !file.open( QIODevice::ReadWrite | QIODevice::Truncate ) )
    {
        qDebug() << "***ERROR*** open file to write";
        return;
    }
    QByteArray out;

    addString( out, "# vtk DataFile Version 3.0", true );
    addString( out,  "braingl fibers", true );
    addString( out, "BINARY", true );
    addString( out, "DATASET POLYDATA", true );

    addString( out, "POINTS " + QString::number( numPoints ) + " float", true );
    out.append( reinterpret_cast< char* >( pointsOut ), sizeof( float ) * numPoints * 3 );
    addLineDelimiter( out );
    delete[] pointsOut;

    addString( out, "LINES " + QString::number( numLines ) + " " + QString::number( numPoints + numLines ), true );
    out.append( reinterpret_cast< char* >( linesOut ), sizeof( unsigned int ) * ( numPoints + numLines ) );
    addLineDelimiter( out );
    delete[] linesOut;

    QVector< QString >dataNames = dynamic_cast<DatasetFibers*>( m_dataset )->getDataNames();

    if ( dataNames[0] != "no data" )
    {
        addString( out, "POINT_DATA " + QString::number( numPoints ), true );
        addString( out, "FIELD FieldData " + QString::number( dataNames.size() ), true );

        for ( int i = 0; i < dataNames.size(); ++i )
        {
            addString( out, dataNames[i] + " 1 " + QString::number( numPoints ) + " float", true );
            float *dataOut = new float[numPoints];
            QVector<QVector<float> >data = dynamic_cast<DatasetFibers*>( m_dataset )->getData( i );
            pntPosOffset = 0;
            for ( int k = 0; k < data.size(); ++k )
            {
                QVector<float>df = data[k];
                for ( int l = 0; l < df.size(); ++l )
                {
                    dataOut[pntPosOffset++] = df[l];
                }
            }
            switchByteOrderOfArray< float >( dataOut, numPoints );
            out.append( reinterpret_cast< char* >( dataOut ), sizeof( float ) * numPoints );
            addLineDelimiter( out );

            delete[] dataOut;
        }
    }

    file.write( out );

    file.close();
}

void WriterVTK::saveFibsAscii( QString filename )
{
    QVector< QVector<float> >fibs = dynamic_cast<DatasetFibers*>( m_dataset )->getFibs();
    QVector< QVector< QVector<float> > >data = dynamic_cast<DatasetFibers*>( m_dataset )->getData();

    unsigned int numPoints = 0;
    unsigned int numLines = fibs.size();
    for( int i = 0; i < fibs.size(); ++i )
    {
        numPoints += fibs[i].size()/3;
    }

    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        qDebug() << "***ERROR*** open file to write";
        return;
    }
    QTextStream out( &file );

    out << "# vtk DataFile Version 3.0" << "\n";
    out << "braingl fibers" << "\n";
    out << "ASCII" << "\n";
    out << "DATASET POLYDATA" << "\n";

    out << QString( "POINTS " + QString::number( numPoints ) + " float" ) << "\n";
    int lc = 0;
    for ( int i = 0; i <  fibs.size(); ++i )
    {
        QVector<float>fib = fibs[i];
        for ( int k = 0; k < fib.size();++k )
        {
            out << QString::number( fib[k] );
            ++lc;
            if ( lc == 9 )
            {
                out << "\n";
                lc = 0;
            }
            else
            {
                out << " ";
            }
        }
    }
    out << "\n";
    out << "\n";

    out << "LINES " + QString::number( numLines ) + " " + QString::number( numPoints + numLines ) << "\n";
    lc = 0;
    for ( int i = 0; i <  fibs.size(); ++i )
    {
        QVector<float>fib = fibs[i];
        out << QString::number( fib.size() / 3 ) << " ";
        for ( int k = 0; k < fib.size() / 3 ;++k )
        {
            out << QString::number( lc++ ) << " ";
        }
        out << "\n";
    }

    out << "\n";

//    out << "POINT_DATA " << QString::number( numPoints );
//    out << "COLOR_SCALARS Colors 3" << "\n";

    QVector< QString >dataNames = dynamic_cast<DatasetFibers*>( m_dataset )->getDataNames();

    if ( dataNames[0] != "no data" )
    {
        out << "POINT_DATA " + QString::number( numPoints ) << "\n";
        out << "FIELD FieldData " + QString::number( dataNames.size() ) << "\n";

        for ( int i = 0; i < dataNames.size(); ++i )
        {
            out << dataNames[i] + " 1 " + QString::number( numPoints ) + " float" << "\n";

            QVector<QVector<float> >data = dynamic_cast<DatasetFibers*>( m_dataset )->getData( i );
            lc = 0;

            for ( int l = 0; l <  fibs.size(); ++l )
            {
                QVector<float>fib = data[l];
                for ( int k = 0; k < fib.size();++k )
                {
                    out << QString::number( fib[k] );
                    ++lc;
                    if ( lc == 9 )
                    {
                        out << "\n";
                        lc = 0;
                    }
                    else
                    {
                        out << " ";
                    }
                }
            }
            out << "\n";
        }
    }

    file.close();
}

void WriterVTK::saveMeshVTK( QString filename, TriangleMesh2* mesh )
{
    int bufferSize = mesh->bufferSize();
    float* points = mesh->getVertices();
    int* indexes = mesh->getIndexes();
    float* colors = mesh->getVertexColors();

    int numPoints = mesh->numVerts();
    int numTris = mesh->numTris();

    float* pointsOut = new float[ numPoints * 3 ];
    char* colorsOut = new char[ numPoints * 3 ];
    int* trisOut = new int[numTris*4];

    if ( m_dataset->properties()->get( Fn::Property::D_INVERT_VERTEX_ORDER ).toBool() )
    {
        for ( int i = 0; i < numTris; ++i )
        {
            trisOut[ 4 * i    ] = 3;
            trisOut[4 * i + 1 ] = indexes[ 3 * i ];
            trisOut[4 * i + 2 ] = indexes[ 3 * i + 2];
            trisOut[4 * i + 3 ] = indexes[ 3 * i + 1];
        }
    }
    else
    {
        for ( int i = 0; i < numTris; ++i )
        {
            trisOut[ 4 * i    ] = 3;
            trisOut[4 * i + 1 ] = indexes[ 3 * i ];
            trisOut[4 * i + 2 ] = indexes[ 3 * i + 1];
            trisOut[4 * i + 3 ] = indexes[ 3 * i + 2];
        }
    }

    for ( int i = 0; i < numPoints; ++i )
    {
        pointsOut[3*i  ] = points[bufferSize * i];
        pointsOut[3*i+1] = points[bufferSize * i+1];
        pointsOut[3*i+2] = points[bufferSize * i+2];
        colorsOut[3*i  ] = colors[4*i] * 255;
        colorsOut[3*i+1] = colors[4*i+1] * 255;
        colorsOut[3*i+2] = colors[4*i+2] * 255;
    }

    switchByteOrderOfArray< float >( pointsOut, numPoints * 3 );
    //switchByteOrderOfArray< float >( colorsOut, numPoints * 3 );
    switchByteOrderOfArray< int >( trisOut, numTris * 4 );


    QFile file( filename );
    if ( !file.open( QIODevice::ReadWrite | QIODevice::Truncate ) )
    {
        qDebug() << "***ERROR*** open file to write";
        return;
    }
    QByteArray out;

    addString( out, "# vtk DataFile Version 3.0", true );
    addString( out, "braingl Mesh", true );
    addString( out, "BINARY", true );
    addString( out, "DATASET POLYDATA", true );

    addString( out, "POINTS " + QString::number( numPoints ) + " float", true );
    out.append( reinterpret_cast< char* >( pointsOut ), sizeof( float ) * numPoints * 3 );
    addLineDelimiter( out );

    addString( out, "POLYGONS " + QString::number( numTris ) + " " + QString::number( numTris * 4 ), true );
    out.append( reinterpret_cast< char* >( trisOut ), sizeof( int ) * ( numTris * 4 ) );
    addLineDelimiter( out );

    addString( out, "POINT_DATA " + QString::number( numPoints ), true );
    addString( out, "COLOR_SCALARS Colors 3", true );
    out.append( colorsOut, numPoints * 3 );
    addLineDelimiter( out );

    file.write( out );

    file.close();

    delete[] pointsOut;
    delete[] colorsOut;
    delete[] trisOut;
}



void WriterVTK::addLineDelimiter( QByteArray& out )
{
    const char lineDelimiter = '\n';
    out.append( lineDelimiter );
}

void WriterVTK::addString( QByteArray& out, QString string, bool withLineDelimiter )
{
    out.append( string );
    if ( withLineDelimiter )
    {
        addLineDelimiter( out );
    }
}
