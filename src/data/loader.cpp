/*
 * Loader.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>
#include <QtGui/QVector3D>

#include "datasets/datasetscalar.h"
#include "datasets/dataset3d.h"
#include "datasets/datasetdwi.h"

#include "loader.h"

Loader::Loader( QDir fileName ) :
    m_fileName( fileName ),
    m_datasetType( FNDT_UNKNOWN ),
    m_success( false )
{
}

Loader::~Loader()
{
}

bool Loader::succes()
{
	return m_success;
}

Dataset* Loader::getDataset()
{
	return m_dataset;
}

bool Loader::load()
{
    switch ( determineType() )
    {
        case FNDT_NIFTI_SCALAR:
        {
        	void* data = loadNifti( m_fileName.path() );
            DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }
        case FNDT_NIFTI_VECTOR:
        {
            void* data = loadNifti( m_fileName.path() );
            Dataset3D* dataset = new Dataset3D( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }
        case FNDT_NIFTI2_SCALAR:
        {
            QString fn = m_fileName.path();
            if ( m_fileName.path().endsWith(".hdr") )
            {
                QDir dir;
                fn.replace( fn.size() - 3, 3, "img" );
                if ( !dir.exists( fn ) )
                {
                    fn.replace( fn.size() - 3, 3, "ima" );
                    if ( !dir.exists( fn ) )
                    {
                        qDebug() << "no image file found";
                    }
                }
            }

            void* data = loadNifti( fn );
            DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }
        case FNDT_NIFTI2_VECTOR:
        {
            QString fn = m_fileName.path();
            if ( m_fileName.path().endsWith(".hdr") )
            {
                QDir dir;
                fn.replace( fn.size() - 3, 3, "img" );
                if ( !dir.exists( fn ) )
                {
                    fn.replace( fn.size() - 3, 3, "ima" );
                    if ( !dir.exists( fn ) )
                    {
                        qDebug() << "no image file found";
                    }
                }
            }

            void* data = loadNifti( m_fileName.path() );
            Dataset3D* dataset = new Dataset3D( m_fileName.path(), data );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;
            return true;
            break;
        }

        case FNDT_NIFTI_DWI:
        {
            loadDWI( m_fileName.path() );
            return true;
            break;
        }
        default:
            break;
    }
    return false;
}

FN_DATASET_TYPE Loader::determineType()
{
    if ( m_fileName.path().endsWith(".nii.gz") || m_fileName.path().endsWith(".nii") )
    {
        m_header = nifti_image_read( m_fileName.path().toStdString().c_str(), 0 );

        // if a proper nifti header is found
        if ( m_header )
        {
            if ( m_header->dim[4] == 1 )
            {
                return FNDT_NIFTI_SCALAR;
            }
            else if ( m_header->dim[4] == 3 )
            {
                return FNDT_NIFTI_VECTOR;
            }
            else if ( m_header->dim[4] > 3 )
            {
                QString fn = m_fileName.path();
                bool hasBVal = false;
                bool hasBVec = false;

                fn.replace( ".nii.gz", ".bval" );
                fn.replace( ".nii", ".bval" );
                QDir dir( fn );
                if ( dir.exists( dir.absolutePath() ) )
                {
                    hasBVal = true;
                }
                fn.replace( ".bval", ".bvec" );
                QDir dir2( fn );
                if ( dir2.exists( dir2.absolutePath() ) )
                {
                    hasBVec = true;
                }

                if ( hasBVal && hasBVec )
                {
                    return FNDT_NIFTI_DWI;
                }
            }
        }
    }
    else if ( m_fileName.path().endsWith(".hdr") || m_fileName.path().endsWith(".ima") || m_fileName.path().endsWith(".img") )
    {
        if ( m_fileName.path().endsWith(".hdr") )
        {
            m_header = nifti_image_read( m_fileName.path().toStdString().c_str(), 0 );
        }
        else
        {
            QString fn = m_fileName.path();
            fn.replace( fn.size() - 3, 3, "hdr" );
            m_header = nifti_image_read( fn.toStdString().c_str(), 0 );
        }
        if ( m_header )
        {
            if ( m_header->dim[4] == 1 )
            {
                return FNDT_NIFTI2_SCALAR;
            }
            else if ( m_header->dim[4] == 3 )
            {
                return FNDT_NIFTI2_VECTOR;
            }
            else if ( m_header->dim[4] > 3 )
            {
                return FNDT_NIFTI2_DWI;
            }
        }
    }
    return FNDT_UNKNOWN;
}

void* Loader::loadNifti( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );

    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    size_t dim = m_header->dim[4];

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* data = new uint8_t[ blockSize * dim ];

            uint8_t* inputData;
            inputData = reinterpret_cast<uint8_t*>( filedata->data );

            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            nifti_image_free( filedata );
            return data;
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* data = new int16_t[ blockSize * dim ];
            int16_t* inputData;
            inputData = reinterpret_cast<int16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            nifti_image_free( filedata );
            return data;
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* data = new int32_t[ blockSize * dim ];
            int32_t* inputData;
            inputData = reinterpret_cast<int32_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            nifti_image_free( filedata );
            return data;
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* data = new float[ blockSize * dim ];
            float* inputData;
            inputData = reinterpret_cast<float*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            nifti_image_free( filedata );
            return data;
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* data = new int8_t[ blockSize * dim ];
            int8_t* inputData;
            inputData = reinterpret_cast<int8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            return data;
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* data = new uint16_t[ blockSize * dim ];
            uint16_t* inputData;
            inputData = reinterpret_cast<uint16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                for ( size_t j = 0; j < dim; ++j )
                {
                    data[i * dim + j] = inputData[j * blockSize + i];
                }
            }
            nifti_image_free( filedata );
            return data;
            break;
        }
    }

    return 0;
}

void Loader::loadDWI( QString fileName )
{
    QStringList slBvals;
    QStringList slBvecs;

    int numB0 = 0;
    QVector<int> bvals;
    QVector<int>bvals2;
    QVector<QVector3D> bvecs;

    QString fn = m_fileName.path();
    fn.replace( ".nii.gz", ".bval" );
    fn.replace( ".nii", ".bval" );
    QDir dir( fn );
    if ( dir.exists( dir.absolutePath() ) )
    {
        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            return;
        }

        QTextStream in( &file );
        if ( !in.atEnd() )
        {
            slBvals = in.readLine().split( " " );
            for ( int i = 0; i < slBvals.size(); ++i )
            {
                bvals.push_back( slBvals[i].toInt() );
            }

        }
    }

    fn.replace( ".bval", ".bvec" );
    QDir dir2( fn );
    if ( dir2.exists( dir2.absolutePath() ) )
    {
        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            return;
        }

        QTextStream in( &file );
        while ( !in.atEnd() )
        {
            slBvecs.push_back( in.readLine() );
        }
        QString sX = slBvecs[0];
        QString sY = slBvecs[1];
        QString sZ = slBvecs[2];
        QStringList slX = sX.split( " " );
        QStringList slY = sY.split( " " );
        QStringList slZ = sZ.split( " " );

        for ( int i = 0; i < slX.size(); ++i )
        {
            if ( bvals[i] != 0 )
            {
                bvecs.push_back( QVector3D( slX[i].toFloat(), slY[i].toFloat(), slZ[i].toFloat() ) );
                bvals2.push_back( bvals[i] );
            }
            else
            {
                ++numB0;
            }
        }
    }

    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    size_t dim = m_header->dim[4];

    int numData = dim - numB0;

    QVector<ColumnVector>* dataVector = new QVector<ColumnVector>();

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_INT16:
        {
            int16_t* data = new int16_t[ blockSize * numData ];
            QVector<float> b0data( blockSize );
            int16_t* inputData;

            int b0Index = 0;

            inputData = reinterpret_cast<int16_t*>( filedata->data );

            for ( int i = 0; i < dim; ++i )
            {
                if ( bvals[i] == 0 )
                {
                    qDebug() << "extract b0 at image " << i;
                    for ( int j = 0; j < blockSize; ++j )
                    {
                        b0data[ j ] += inputData[ i * blockSize + j ] / numB0;
                    }
                    ++b0Index;
                }
            }

            qDebug() << "extract data ";
            for ( int i = 0; i < blockSize; ++i )
            {
                ColumnVector v( numData );
                int dataIndex = 1;
                for ( int j = 0; j < dim; ++j )
                {
                    if ( bvals[j] != 0 )
                    {
                        v( dataIndex ) = inputData[ j * blockSize + i ];
                        ++dataIndex;
                    }
                }
                dataVector->push_back( v );
            }
            qDebug() << "extract data done";

            DatasetDWI* dataset = new DatasetDWI( m_fileName.path(), dataVector, b0data, bvals2, bvecs );
            dataset->parseNiftiHeader( m_header );
            dataset->examineDataset();
            m_dataset = dataset;

            nifti_image_free( filedata );
            break;
        }
    }

    qDebug() << "end loading data";

    //qDebug() << bvals2;
    //qDebug() << bvecs;


}
