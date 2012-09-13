/*
 * Loader.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtGui/QVector3D>
#include <QtGui/QtGui>

#include "datasets/datasetscalar.h"
#include "datasets/dataset3d.h"
#include "datasets/datasetdwi.h"
#include "datasets/datasettensor.h"
#include "mesh/trianglemesh.h"

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
    QString fn = m_fileName.path();

    if ( fn.endsWith( ".nii.gz" ) || fn.endsWith( ".nii" ) || fn.endsWith(".hdr") || fn.endsWith(".ima") || fn.endsWith(".img") )
    {

        return loadNifti();
    }

    if ( m_fileName.path().endsWith(".mesh") )
    {
        return loadMesh();
    }

    return false;
}

bool Loader::loadNifti()
{
    QString hdrPath = m_fileName.path();
    QString fn = m_fileName.path();
    if ( fn.endsWith(".ima") || fn.endsWith(".img") )
    {
        hdrPath.replace( hdrPath.size() - 3, 3, "hdr" );
    }

    if ( !loadNiftiHeader( hdrPath ) )
    {
        return false;
    }

    if ( m_header->dim[4] == 1 )
    {
        m_datasetType = FNDT_NIFTI_SCALAR;
        return loadNiftiScalar( fn );
    }
    else if ( m_header->dim[4] == 3 )
    {
        m_datasetType = FNDT_NIFTI_VECTOR;
        return loadNiftiVector3D( fn );
    }
    else if ( m_header->dim[4] == 6 )
    {
        m_datasetType = FNDT_NIFTI_TENSOR;
        return loadNiftiTensor( fn );
    }
    else if ( m_header->dim[4] > 3 )
    {
        m_datasetType = FNDT_NIFTI_DWI;
        return loadNiftiDWI( fn );
    }

    return false;
}

bool Loader::loadNiftiHeader( QString hdrPath )
{
    try
    {
        m_header = nifti_image_read( hdrPath.toStdString().c_str(), 0 );
    }
    catch (...)
    {
        // failed to read header
    }
    if ( m_header )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Loader::loadNiftiScalar( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );

    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];

    QVector<float> data( blockSize );

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* inputData;
            inputData = reinterpret_cast<uint8_t*>( filedata->data );

            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* inputData;
            inputData = reinterpret_cast<int16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* inputData;
            inputData = reinterpret_cast<int32_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData;
            inputData = reinterpret_cast<float*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* inputData;
            inputData = reinterpret_cast<int8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* inputData;
            inputData = reinterpret_cast<uint16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i] = static_cast<float>( inputData[i] );
            }
            nifti_image_free( filedata );
            break;
        }
    }

    DatasetScalar* dataset = new DatasetScalar( fileName, data, m_header );
    m_dataset = dataset;
    return true;
}

bool Loader::loadNiftiVector3D( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );

    size_t blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];

    QVector<QVector3D> data( blockSize );

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* inputData;
            inputData = reinterpret_cast<uint8_t*>( filedata->data );

            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* inputData;
            inputData = reinterpret_cast<int16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* inputData;
            inputData = reinterpret_cast<int32_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData;
            inputData = reinterpret_cast<float*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* inputData;
            inputData = reinterpret_cast<int8_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* inputData;
            inputData = reinterpret_cast<uint16_t*>( filedata->data );
            for ( size_t i = 0; i < blockSize; ++i )
            {
                data[i].setX( inputData[i] );
                data[i].setY( inputData[blockSize + i] );
                data[i].setZ( inputData[2*blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
    }

    Dataset3D* dataset = new Dataset3D( fileName, data, m_header );
    m_dataset = dataset;
    return true;
}

bool Loader::loadNiftiTensor( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    int dim = m_header->dim[4];

    QVector<Matrix>* dataVector = new QVector<Matrix>();

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData;

            inputData = reinterpret_cast<float*>( filedata->data );

            for ( int i = 0; i < blockSize; ++i )
            {
                ColumnVector v( dim );
                for ( int j = 0; j < dim; ++j )
                {
                    v( j+1 ) = inputData[ j * blockSize + i ];
                }
                Matrix m( 3, 3 );
                m( 1, 1 ) = v( 1 );
                m( 1, 2 ) = v( 2 );
                m( 1, 3 ) = v( 3 );
                m( 2, 1 ) = v( 2 );
                m( 2, 2 ) = v( 4 );
                m( 2, 3 ) = v( 5 );
                m( 3, 1 ) = v( 3 );
                m( 3, 2 ) = v( 5 );
                m( 3, 3 ) = v( 6 );

                dataVector->push_back( m );
            }

            DatasetTensor* dataset = new DatasetTensor( m_fileName.path(), dataVector, m_header );
            m_dataset = dataset;

            nifti_image_free( filedata );
            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}


bool Loader::loadNiftiDWI( QString fileName )
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
            qDebug() << "couldn't open " << fn;
            return false;
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
    else
    {
        qDebug() << "couldn't open " << fn;
        return false;
    }

    fn.replace( ".bval", ".bvec" );
    QDir dir2( fn );
    if ( dir2.exists( dir2.absolutePath() ) )
    {
        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            qDebug() << "couldn't open " << fn;
            return false;
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
                bvecs.push_back( QVector3D( slX[i].toDouble(), slY[i].toDouble(), slZ[i].toDouble() ) );
                bvals2.push_back( bvals[i] );
            }
            else
            {
                ++numB0;
            }
        }
    }
    else
    {
        qDebug() << "couldn't open " << fn;
        return false;
    }

    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    int dim = m_header->dim[4];

    int numData = dim - numB0;

    QVector<ColumnVector>* dataVector = new QVector<ColumnVector>();

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_INT16:
        {
            QVector<float> b0data( blockSize );
            int16_t* inputData;

            int b0Index = 0;

            inputData = reinterpret_cast<int16_t*>( filedata->data );

            for ( int i = 0; i < dim; ++i )
            {
                if ( bvals[i] == 0 )
                {
                    //qDebug() << "extract b0 at image " << i;
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

            DatasetDWI* dataset = new DatasetDWI( m_fileName.path(), dataVector, b0data, bvals2, bvecs, m_header );
            m_dataset = dataset;

            nifti_image_free( filedata );
            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool Loader::loadMesh()
{
    return loadMeshBinary();
}

bool Loader::loadMeshBinary()
{
    qDebug() << "load mesh";
    QFile file( m_fileName.path() );
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    char* s = new char[29];
    char* s1 = new char[10];
    for ( int i = 0; i < 9; ++i )
    {
        s1[i] = s[i];
    }
    s1[9] = 0;
    QString s2 = QString::fromLocal8Bit( s1 );
    if ( s2 == "binarABCD" )
    {
        in.setByteOrder( QDataStream::BigEndian );
        //qDebug() << "big endian";
    }
    else if ( s2 == "binarDCBA" )
    {
        in.setByteOrder( QDataStream::LittleEndian );
        //qDebug() << "little endian";
    }
    else
    {
        qDebug() << "unknown entry in file";
        return false;
    }
    m_datasetType = FNDT_MESH_BINARY;

    in.setFloatingPointPrecision( QDataStream::SinglePrecision );
    qint32 iVal;
    TriangleMesh* tmesh = new TriangleMesh();
    in >> iVal;
    qDebug() << iVal;
    float x,y,z;
    for ( qint32 i = 0; i < iVal; ++i )
    {
        in >> x;
        in >> y;
        in >> z;
        tmesh->addVertex( x, y, z );
    }
    in >> iVal;
    qDebug() << iVal;
    for ( qint32 i = 0; i < iVal; ++i )
    {
        in >> x;
        in >> y;
        in >> z;
    }
    in >> iVal;
    in >> iVal;
    qDebug() << iVal;
    qint32 v1, v2, v3;
    for ( qint32 i = 0; i < iVal; ++i )
    {
        in >> v1;
        in >> v2;
        in >> v3;
        tmesh->addTriangle( v1, v2, v3 );
    }

    return false;
}
