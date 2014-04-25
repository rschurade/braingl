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

#include <vtkGenericDataObjectReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

LoaderVTK::LoaderVTK( QString fn ) :
    m_filename( fn ),
    m_primitiveType( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 ),
    m_numPolys( 0 ),
    m_hasPointData( false ),
    m_hasPrimitiveData( false ),
    m_hasPointColors( false ),
    m_hasPrimitiveColors( false )
{
    m_status.push_back( "ok" );
}

LoaderVTK::~LoaderVTK()
{
    //m_points->clear();
    m_lines.clear();
    m_polys.clear();
    m_pointData.clear();
    m_pointDataNames.clear();
    m_pointColors.clear();
}

QStringList LoaderVTK::getStatus()
{
    return m_status;
}

int LoaderVTK::getPrimitiveType()
{
    return m_primitiveType;
}

std::vector<float>* LoaderVTK::getPoints()
{
    return m_points;
}

std::vector<int> LoaderVTK::getLines()
{
    return m_lines;
}

std::vector<int> LoaderVTK::getPolys()
{
    return m_polys;
}

std::vector<std::vector<float> > LoaderVTK::getPointData()
{
    return m_pointData;
}

std::vector<unsigned char> LoaderVTK::getPointColors()
{
    return m_pointColors;
}

std::vector<unsigned char> LoaderVTK::getPrimitiveColors()
{
    return m_primitiveColors;
}

QList<QString>LoaderVTK::getPointDataNames()
{
    return m_pointDataNames;
}

int LoaderVTK::getNumPoints()
{
    return m_numPoints;
}

int LoaderVTK::getNumLines()
{
    return m_numLines;
}

int LoaderVTK::getNumPolys()
{
    return m_numPolys;
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
    return true;
}

bool LoaderVTK::exists()
{
    QDir dir( m_filename );

    if ( !dir.exists( dir.absolutePath() ) )
    {
        m_status.push_back(  "*ERROR* file " + m_filename + " doesn't exist" );
        return false;
    }
    else
    {
        return true;
    }
}

bool LoaderVTK::open()
{
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName( m_filename.toStdString().c_str() );
    reader->Update();

    // All of the standard data types can be checked and obtained like this:
    if ( reader->IsFilePolyData() )
    {
        qDebug() << "vtk file is a polydata";
        vtkPolyData* output = reader->GetPolyDataOutput();

        m_numPoints = output->GetNumberOfPoints();
        m_numLines = output->GetNumberOfLines();
        m_numPolys = output->GetNumberOfPolys();

        qDebug() << "vtk file has " << m_numPoints << " points.";
        qDebug() << "vtk file has " << m_numLines << " lines.";
        qDebug() << "vtk file has " << m_numPolys << " polys.";

        if( m_numPoints <= 0 )
        {
            return false;
        }

        if ( m_numPolys > 0 )
        {
            m_primitiveType = 1;
        }
        else if ( m_numLines > 0 )
        {
            m_primitiveType = 2;
        }
        else
        {
            return false;
        }

        double p[3];
        m_points = new std::vector<float>();
        m_points->reserve( m_numPoints * 3 );
        for ( vtkIdType i = 0; i < output->GetNumberOfPoints(); ++i )
        {
            output->GetPoint( i, p );
            m_points->push_back( p[0] );
            m_points->push_back( p[1] );
            m_points->push_back( p[2] );
        }
        if ( m_numPolys > 0 )
        {
            output->GetPolys()->InitTraversal();
            vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
            while ( output->GetPolys()->GetNextCell( idList ) )
            {
                m_polys.push_back( idList->GetNumberOfIds() );
                for ( vtkIdType pointId = 0; pointId < idList->GetNumberOfIds(); ++pointId )
                {
                    m_polys.push_back( idList->GetId( pointId ) );
                }
            }
        }

        if ( m_numLines > 0 )
        {
            m_lines.reserve( m_numLines + m_numPoints );
            output->GetLines()->InitTraversal();
            vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
            while ( output->GetLines()->GetNextCell( idList ) )
            {
                m_lines.push_back( idList->GetNumberOfIds() );
                for ( vtkIdType pointId = 0; pointId < idList->GetNumberOfIds(); ++pointId )
                {
                    m_lines.push_back( idList->GetId( pointId ) );
                }
            }

            ///////// Get cell colors ///////////
            vtkSmartPointer<vtkUnsignedCharArray> array = vtkUnsignedCharArray::SafeDownCast( output->GetCellData()->GetArray("CellColors" ) );
            if ( array )
            {
                m_primitiveColors.reserve( m_numLines *3 );
                for ( int i = 0; i < m_numLines * 3; ++i )
                {
                    m_primitiveColors.push_back( array->GetValue( i ) );
                }
                m_hasPrimitiveColors = true;
            } //end if(array)
            else
            {
                m_primitiveColors.resize( m_numLines * 3 );
                for ( unsigned int i = 0; i < m_primitiveColors.size(); ++i )
                {
                    m_primitiveColors[i] = 255;
                }
                qDebug() << "no cell color array.";
            }
        }
        ///////// Get point colors ///////////
        vtkSmartPointer<vtkUnsignedCharArray> array = vtkUnsignedCharArray::SafeDownCast( output->GetPointData()->GetArray("Colors" ) );

        if ( array )
        {
            m_pointColors.reserve( m_numPoints * 3 );
            for ( int i = 0; i < m_numPoints * 3; ++i )
            {
                m_pointColors.push_back( array->GetValue( i ) );
            }
            m_hasPointColors = true;
        } //end if(array)
        else
        {
            m_pointColors.resize( m_numPoints * 3 );
            for ( unsigned int i = 0; i < m_pointColors.size(); ++i )
            {
                m_pointColors[i] = 255;
            }
            qDebug() << "no point color array.";
        }

        unsigned int numberOfArrays = output->GetPointData()->GetNumberOfArrays();
        qDebug() << "NumArrays: " << numberOfArrays;
        //qDebug() << "key: ";
        //more values can be found in <VTK_DIR>/Common/vtkSetGet.h
//        qDebug() << VTK_UNSIGNED_CHAR << " unsigned char";
//        qDebug() << VTK_UNSIGNED_INT << " unsigned int";
//        qDebug() << VTK_FLOAT << " float";
//        qDebug() << VTK_DOUBLE << " double";

        for ( unsigned int i = 0; i < numberOfArrays; ++i )
        {
            //the following two lines are equivalent
            //arrayNames.push_back(polydata->GetPointData()->GetArray(i)->GetName());

            int dataTypeID = output->GetPointData()->GetArray( i )->GetDataType();

//            if ( dataTypeID == VTK_FLOAT )
//            {
                m_pointDataNames.push_back( output->GetPointData()->GetArrayName( i ) );
                qDebug() << "Array " << i << ": " << m_pointDataNames.back() << " (type: " << dataTypeID << ")";

                std::vector<float>data( m_numPoints );
                //vtkSmartPointer<vtkFloatArray> dataArray = vtkFloatArray::SafeDownCast( output->GetPointData()->GetArray( i ) );
                vtkSmartPointer<vtkDataArray> dataArray = output->GetPointData()->GetArray( i );
                if ( dataArray )
                {
                    for ( int k = 0; k < m_numPoints; ++k )
                    {
                        data[k] = dataArray->GetVariantValue( k ).ToFloat();
                    }
                    m_hasPointData = true;
                }
                m_pointData.push_back( data );
 //           }
        }

        numberOfArrays = output->GetCellData()->GetNumberOfArrays();
        qDebug() << "NumCellArrays: " << numberOfArrays;

        for ( unsigned int i = 0; i < numberOfArrays; ++i )
        {
            //the following two lines are equivalent
            //arrayNames.push_back(polydata->GetPointData()->GetArray(i)->GetName());

            int dataTypeID = output->GetCellData()->GetArray( i )->GetDataType();

//            if ( dataTypeID == VTK_FLOAT )
//            {
                m_pointDataNames.push_back( output->GetCellData()->GetArrayName( i ) );
                qDebug() << "Array " << i << ": " << m_pointDataNames.back() << " (type: " << dataTypeID << ")";

                std::vector<float>data;
                data.reserve( m_numPoints );
                //vtkSmartPointer<vtkFloatArray> dataArray = vtkFloatArray::SafeDownCast( output->GetCellData()->GetArray( i ) );
                vtkSmartPointer<vtkDataArray> dataArray = output->GetCellData()->GetArray( i );
                if ( dataArray )
                {
                    int lc = 0;
                    for ( int k = 0; k < m_numLines; ++k )
                    {
                        int lineSize = m_lines[lc];
                        for ( int l = 0; l < lineSize; ++l )
                        {
                            data.push_back( dataArray->GetVariantValue( k ).ToFloat() );
                        }
                    }

                    m_hasPointData = true;
                }
                m_pointData.push_back( data );
//            }
        }

        return true;
    }
    return false;
}
