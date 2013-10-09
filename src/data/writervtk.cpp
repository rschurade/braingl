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
        switch ( (Fn::DatasetType)( m_dataset->properties()->get( Fn::Property::D_TYPE ).toInt() ) )
        {
            qDebug() << m_filter;
            case Fn::DatasetType::FIBERS:
            {
                if ( m_filter.startsWith( "fib files ascii") )
                {
                    saveFibs( m_fileName, false );
                }
                else
                {
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
    QVector< QVector<float> >fibs = dynamic_cast<DatasetFibers*>( m_dataset )->getFibs();
    QVector< QVector< QVector<float> > >data = dynamic_cast<DatasetFibers*>( m_dataset )->getData();
    QVector< QString >dataNames = dynamic_cast<DatasetFibers*>( m_dataset )->getDataNames();

    vtkSmartPointer<vtkPolyData> newPolyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    vtkIdType pointId = 0;

    for ( int i = 0; i < fibs.size(); ++i )
    {
        QVector<float>fib = fibs[i];
        vtkSmartPointer<vtkPolyLine> newLine = vtkSmartPointer<vtkPolyLine>::New();
        for (int k = 0; k < fib.size() / 3; ++k )
        {
            newPoints->InsertNextPoint( fib[k*3], fib[k*3+1], fib[k*3+2] );
            newLine->GetPointIds()->InsertNextId( pointId );
            ++pointId;
        }
         newLines->InsertNextCell( newLine );
    }
     // add points and lines to new polydata
     newPolyData->SetPoints( newPoints );
     newPolyData->SetLines( newLines );

     if ( dataNames[0] != "no data" )
     {
         for ( int i = 0; i < data.size(); ++i )
         {
             vtkSmartPointer<vtkFloatArray> dataArray = vtkSmartPointer<vtkFloatArray>::New();
             dataArray->SetNumberOfComponents( 1 );
             dataArray->SetName( dataNames[i].toStdString().c_str() );

             QVector< QVector<float> >dataField = data[i];
             for( int k = 0; k < dataField.size(); ++k )
             {
                 QVector<float>fib = dataField[k];
                 for( int l = 0; l < fib.size(); ++l )
                 {
                     dataArray->InsertNextValue( fib[l] );
                 }
             }
             newPolyData->GetPointData()->AddArray( dataArray );
         }
     }

     qDebug() << "Writing "  << filename;
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
     writer->SetInput( newPolyData );
     if ( ! writer->Write() )
     {
         qDebug() << "Error writing " << filename;
     }


}

void WriterVTK::saveMesh( QString filename, TriangleMesh2* mesh, bool binary )
{
    int bufferSize = mesh->bufferSize();
    float* points = mesh->getVertices();
    int* indexes = mesh->getIndexes();
    float* colors = mesh->getVertexColors();

    int numPoints = mesh->numVerts();
    int numTris = mesh->numTris();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();

    for ( int i = 0; i < numPoints; ++i )
    {
        newPoints->InsertNextPoint( points[i*bufferSize], points[i*bufferSize+1], points[i*bufferSize+2] );
    }

    if ( m_dataset->properties()->get( Fn::Property::D_INVERT_VERTEX_ORDER ).toBool() )
    {
        for( int i = 0; i < numTris; ++i )
        {
            polys->InsertNextCell(3);
            polys->InsertCellPoint( indexes[i*3] );
            polys->InsertCellPoint( indexes[i*3+2] );
            polys->InsertCellPoint( indexes[i*3+1] );
        }

    }
    else
    {
        for( int i = 0; i < numTris; ++i )
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

    for ( int i = 0; i < numPoints; ++i )
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

    for ( int i = 0; i < numPoints; ++i )
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
    writer->SetInput( polyData );
    if ( !writer->Write() )
    {
        qDebug() << "Error writing " << filename;
    }
}
