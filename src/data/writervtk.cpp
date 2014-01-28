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

#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPolyLine.h>


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
        //qDebug() << "filter:" << m_filter;
        switch ( (Fn::DatasetType)( m_dataset->properties()->get( Fn::Property::D_TYPE ).toInt() ) )
        {
            case Fn::DatasetType::FIBERS:
            {
                if ( m_filter.startsWith( "fib files ascii") )
                {
                    qDebug() << "save ascii";
                    saveFibs( m_fileName, false );
                }
                else
                {
                    qDebug() << "save binary";
                    saveFibs( m_fileName, true );
                }
                break;
            }
            case Fn::DatasetType::MESH_BINARY:
            case Fn::DatasetType::MESH_ISOSURFACE:
            case Fn::DatasetType::MESH_TIME_SERIES :
            case Fn::DatasetType::GLYPHSET :
            {
                TriangleMesh2* mesh = dynamic_cast<DatasetMesh*>( m_dataset )->getMesh();
                if ( m_filter.startsWith( "Mesh ascii") )
                {
                    saveMesh( m_fileName, mesh, false );
                }
                else if ( m_filter.startsWith( "Mesh binary") )
                {
                    saveMesh( m_fileName, mesh, true );
                }
                break;
            }
            default:
                break;
        }
    }
    return true;
}

void WriterVTK::saveFibs( QString filename, bool binary )
{
    DatasetFibers* ds = dynamic_cast<DatasetFibers*>( m_dataset );
    std::vector< Fib >* fibs = ds->getFibs();
    QList< QString >dataNames = ds->getDataNames();

    vtkSmartPointer<vtkPolyData> newPolyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType pointId = 0;

    vtkSmartPointer<vtkUnsignedCharArray> colorArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorArray->SetNumberOfComponents(3);
    colorArray->SetName ("CellColors");

    unsigned char rgb[3];

    for ( unsigned int i = 0; i < fibs->size(); ++i )
    {
        Fib fib = fibs->at( i );
        vtkSmartPointer<vtkPolyLine> newLine = vtkSmartPointer<vtkPolyLine>::New();
        QColor color = fib.customColor();

        rgb[0] = color.redF() * 255;
        rgb[1] = color.greenF() * 255;
        rgb[2] = color.blueF() * 255;

        colorArray->InsertNextTupleValue( rgb );

        for ( unsigned int k = 0; k < fib.length(); ++k )
        {
            newPoints->InsertNextPoint( fib[k].x(), fib[k].y(), fib[k].z() );
            newLine->GetPointIds()->InsertNextId( pointId );
            ++pointId;
        }
        newLines->InsertNextCell( newLine );
    }

    // add points and lines to new polydata
    newPolyData->SetPoints( newPoints );
    newPolyData->SetLines( newLines );
    newPolyData->GetCellData()->AddArray( colorArray );

    if ( dataNames[0] != "no data" )
    {
        for ( int i = 0; i < dataNames.size(); ++i )
        {
            vtkSmartPointer<vtkFloatArray> dataArray = vtkSmartPointer<vtkFloatArray>::New();
            dataArray->SetNumberOfComponents( 1 );
            dataArray->SetName( dataNames[i].toStdString().c_str() );

            for ( unsigned int k = 0; k < fibs->size(); ++k )
            {
                Fib fib = fibs->at( k );
                for ( unsigned int l = 0; l < fib.length(); ++l )
                {
                    dataArray->InsertNextValue( fib.getData( k, l ) );
                }
            }
            newPolyData->GetPointData()->AddArray( dataArray );
        }
    }

    qDebug() << "Writing " << filename;
    vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
    writer->SetFileName( filename.toStdString().c_str() );

    if ( binary )
    {
        writer->SetFileTypeToBinary();
    }
    else
    {
        writer->SetFileTypeToASCII();
    }

#if VTK_MAJOR_VERSION < 6
    writer->SetInput( newPolyData );
#else
    writer->SetInputData( newPolyData );
#endif

     if ( ! writer->Write() )
     {
         qDebug() << "Error writing " << filename;
     }
}

void WriterVTK::saveMesh( QString filename, TriangleMesh2* mesh, bool binary )
{
    int bufferSize = mesh->bufferSize();
    float* points = mesh->getVertices();
    unsigned int* indexes = mesh->getIndexes();
    float* colors = mesh->getVertexColors();

    unsigned int numPoints = mesh->numVerts();
    unsigned int numTris = mesh->numTris();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();

    for ( unsigned int i = 0; i < numPoints; ++i )
    {
        newPoints->InsertNextPoint( points[i*bufferSize], points[i*bufferSize+1], points[i*bufferSize+2] );
    }

    if ( m_dataset->properties()->get( Fn::Property::D_INVERT_VERTEX_ORDER ).toBool() )
    {
        for( unsigned int i = 0; i < numTris; ++i )
        {
            polys->InsertNextCell(3);
            polys->InsertCellPoint( indexes[i*3] );
            polys->InsertCellPoint( indexes[i*3+2] );
            polys->InsertCellPoint( indexes[i*3+1] );
        }

    }
    else
    {
        for( unsigned int i = 0; i < numTris; ++i )
        {
            polys->InsertNextCell(3);
            polys->InsertCellPoint( indexes[i*3] );
            polys->InsertCellPoint( indexes[i*3+1] );
            polys->InsertCellPoint( indexes[i*3+2] );
        }

    }

    vtkSmartPointer<vtkUnsignedCharArray> colorArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorArray->SetNumberOfComponents(3);
    colorArray->SetName ("Colors");

    unsigned char rgb[3];

    for ( unsigned int i = 0; i < numPoints; ++i )
    {
        rgb[0] = colors[4*i]   * 255;
        rgb[1] = colors[4*i+1] * 255;
        rgb[2] = colors[4*i+2] * 255;

        colorArray->InsertNextTupleValue( rgb );
    }

    // add points and lines to new polydata
    polyData->SetPoints( newPoints );
    polyData->SetPolys( polys );
    polyData->GetPointData()->SetScalars( colorArray );

    vtkSmartPointer<vtkFloatArray> dataArray = vtkSmartPointer<vtkFloatArray>::New();
    dataArray->SetNumberOfComponents( 1 );
    dataArray->SetName( "data" );

    for ( unsigned int i = 0; i < numPoints; ++i )
    {
        dataArray->InsertNextValue( points[i*bufferSize+6] );
    }
    polyData->GetPointData()->AddArray( dataArray );

    qDebug() << "Writing " << filename;
    vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
    writer->SetFileName( filename.toStdString().c_str() );

    if ( binary )
    {
        writer->SetFileTypeToBinary();
    }
    else
    {
        writer->SetFileTypeToASCII();
    }
#if VTK_MAJOR_VERSION < 6
    writer->SetInput( polyData );
#else
    writer->SetInputData( polyData );
#endif
    if ( !writer->Write() )
    {
        qDebug() << "Error writing " << filename;
    }
}
