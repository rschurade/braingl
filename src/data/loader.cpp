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
#include "datasets/datasetsh.h"
#include "mesh/trianglemesh2.h"

#include "loader.h"

Loader::Loader( QDir fileName ) :
        m_fileName( fileName ), m_datasetType( FNDT_UNKNOWN ), m_success( false )
{
}

Loader::~Loader()
{
}

bool Loader::succes()
{
    return m_success;
}

Dataset* Loader::getDataset( int id )
{
    return m_dataset[id];
}

int Loader::getNumDatasets()
{
    return m_dataset.size();
}

bool Loader::load()
{
    QString fn = m_fileName.path();

    if ( fn.endsWith( ".nii.gz" ) || fn.endsWith( ".nii" ) || fn.endsWith( ".hdr" ) || fn.endsWith( ".ima" ) || fn.endsWith( ".img" ) )
    {

        return loadNifti();
    }

    if ( m_fileName.path().endsWith( ".mesh" ) )
    {
        return loadMesh();
    }

    return false;
}

bool Loader::loadNifti()
{
    QString hdrPath = m_fileName.path();
    QString fn = m_fileName.path();
    if ( fn.endsWith( ".ima" ) || fn.endsWith( ".img" ) )
    {
        hdrPath.replace( hdrPath.size() - 3, 3, "hdr" );
    }

    if ( !loadNiftiHeader( hdrPath ) )
    {
        return false;
    }

    if ( m_header->dim[4] == 1 || m_header->dim[4] == 0 )
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
    else if ( m_header->dim[4] == 15 || m_header->dim[4] == 28 || m_header->dim[4] == 45 )
    {
        m_datasetType = FNDT_NIFTI_SH;
        return loadNiftiQBall( fn );
    }
    else if ( m_header->dim[4] > 3 )
    {
        if ( QString( m_header->descrip ) == QString( "fnav2_dwi" ) )
        {
            qDebug() << "fnav2 dwi dataset found";
            m_datasetType = FNDT_NIFTI_DWI;
            return loadNiftiDWI_FNAV2( fn );
        }
        else
        {
            m_datasetType = FNDT_NIFTI_DWI;
            return loadNiftiDWI( fn );
        }
    }

    return false;
}

bool Loader::loadNiftiHeader( QString hdrPath )
{
    try
    {
        m_header = nifti_image_read( hdrPath.toStdString().c_str(), 0 );
    }
    catch ( ... )
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
    m_dataset.push_back( dataset );
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
                data[i].setZ( inputData[2 * blockSize + i] );
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
                data[i].setZ( inputData[2 * blockSize + i] );
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
                data[i].setZ( inputData[2 * blockSize + i] );
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
                data[i].setZ( inputData[2 * blockSize + i] );
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
                data[i].setZ( inputData[2 * blockSize + i] );
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
                data[i].setZ( inputData[2 * blockSize + i] );
            }
            nifti_image_free( filedata );
            break;
        }
    }

    Dataset3D* dataset = new Dataset3D( fileName, data, m_header );
    m_dataset.push_back( dataset );
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
                    v( j + 1 ) = inputData[j * blockSize + i];
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
            nifti_image_free( filedata );

            DatasetTensor* dataset = new DatasetTensor( m_fileName.path(), dataVector, m_header );
            m_dataset.push_back( dataset );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool Loader::loadNiftiQBall( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int blockSize = m_header->dim[1] * m_header->dim[2] * m_header->dim[3];
    int dim = m_header->dim[4];
    int order = 4;
    if ( dim == 15 )
    {
        order = 4;
    }
    else if ( dim == 28 )
    {
        order = 6;
    }
    else // dim == 45
    {
        order = 8;
    }

    QVector<ColumnVector>* dataVector = new QVector<ColumnVector>();

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
                    v( j + 1 ) = inputData[j * blockSize + i];
                }
                dataVector->push_back( v );
            }
            nifti_image_free( filedata );

            DatasetSH* out = new DatasetSH( m_fileName.path(), dataVector, m_header );
            //out->setProperty( "name", "QBall" );
            out->setProperty( "createdBy", FNALGO_QBALL );
            out->setProperty( "lod", 2 );
            out->setProperty( "order", order );
            out->setProperty( "renderSlice", 1 );

            m_dataset.push_back( out );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool Loader::loadNiftiDWI( QString fileName )
{
    QVector<float> bvals = loadBvals( fileName );

    if ( bvals.size() == 0 || bvals.size() != m_header->dim[4] )
    {
        qDebug() << "*** ERROR *** while loading dwi dataset, count bvals doesn't match nifti image dim!";
        return false;
    }
    int numB0 = 0;
    QVector<float> bvals2;
    for ( int i = 0; i < bvals.size(); ++i )
    {
        if ( bvals[i] > 100 )
        {
            bvals2.push_back( bvals[i] );
        }
        else
        {
            ++numB0;
        }
    }
    qDebug() << "num b0:" << numB0;

    QVector<QVector3D> bvecs = loadBvecs( fileName, bvals );
    if ( bvecs.size() == 0 )
    {
        qDebug() << "*** ERROR *** while loading bvecs!";
        return false;
    }

    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];
    int numData = dim - numB0;
    qDebug() << "num data:" << numData;

    if ( numData > dim || bvals2.size() != bvecs.size() )
    {
        qDebug() << "*** ERROR *** parsing bval and bvec files!";
        return false;
    }

    QVector<ColumnVector>* dataVector = new QVector<ColumnVector>();

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_INT16:
        {
            QVector<float> b0data( blockSize );
            qDebug() << "block size: " << blockSize;
            int16_t* inputData;

            inputData = reinterpret_cast<int16_t*>( filedata->data );

            nifti_image* b0Hdr = nifti_copy_nim_info( m_header );

            qDebug() << "extract data ";

            dataVector->reserve( blockSize );
            if ( m_header->qto_xyz.m[0][0] < 0 || m_header->sto_xyz.m[0][0] < 0 )
            {
                qDebug() << fileName << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = dimX - 1; x >= 0; --x )
                        {
                            ColumnVector v( numData );
                            int dataIndex = 1;
                            for ( int j = 0; j < dim; ++j )
                            {
                                if ( bvals[j] > 100 )
                                {
                                    v( dataIndex ) = inputData[j * blockSize + x + y * dimX + z * dimX * dimY];
                                    ++dataIndex;
                                }
                                else
                                {
                                    b0data[( dimX - x - 1 ) + y * dimX + z * dimX * dimY] += inputData[j * blockSize + x + y * dimX + z * dimX * dimY]
                                            / numB0;
                                }
                            }
                            dataVector->push_back( v );
                        }
                    }
                }
                m_header->qto_xyz.m[0][0] = fabs( m_header->qto_xyz.m[0][0] );
                m_header->sto_xyz.m[0][0] = fabs( m_header->sto_xyz.m[0][0] );
                m_header->qto_xyz.m[0][3] = 0.0;
                m_header->sto_xyz.m[0][3] = 0.0;

                b0Hdr->qto_xyz.m[0][0] = fabs( m_header->qto_xyz.m[0][0] );
                b0Hdr->sto_xyz.m[0][0] = fabs( m_header->sto_xyz.m[0][0] );
                b0Hdr->qto_xyz.m[0][3] = 0.0;
                b0Hdr->sto_xyz.m[0][3] = 0.0;

                for ( int i = 0; i < bvecs.size(); ++i )
                {
                    bvecs[i].setX( bvecs[i].x() * -1.0 );
                }
            }
            else
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = 0; x < dimX; ++x )
                        {
                            ColumnVector v( numData );
                            int dataIndex = 1;
                            for ( int j = 0; j < dim; ++j )
                            {
                                if ( bvals[j] > 100 )
                                {
                                    v( dataIndex ) = inputData[j * blockSize + x + y * dimX + z * dimX * dimY];
                                    ++dataIndex;
                                }
                                else
                                {
                                    b0data[x + y * dimX + z * dimX * dimY] += inputData[j * blockSize + x + y * dimX + z * dimX * dimY] / numB0;
                                }
                            }
                            dataVector->push_back( v );
                        }
                    }
                }
            }

            qDebug() << "extract data done";
            nifti_image_free( filedata );

            QString b0fn = m_fileName.path();
            if ( m_fileName.path().endsWith( ".nii.gz" ) )
            {
                b0fn.replace( ".nii.gz", "_b0.nii.gz" );
            }
            if ( m_fileName.path().endsWith( ".nii" ) )
            {
                b0fn.replace( ".nii.gz", "_b0.nii" );
            }

            DatasetScalar* datasetB0 = new DatasetScalar( b0fn, b0data, b0Hdr );
            m_dataset.push_back( datasetB0 );

            nifti_image* dsHdr = nifti_copy_nim_info( m_header );
            DatasetDWI* dataset = new DatasetDWI( m_fileName.path(), dataVector, b0data, bvals2, bvecs, dsHdr );
            m_dataset.push_back( dataset );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool Loader::loadNiftiDWI_FNAV2( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4] - 1;

    qDebug() << "num data:" << dim;

    QVector<ColumnVector>* dataVector = new QVector<ColumnVector>();

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            QVector<float> b0data( blockSize );
            qDebug() << "block size: " << blockSize;
            float* inputData;

            inputData = reinterpret_cast<float*>( filedata->data );

            nifti_image* b0Hdr = nifti_copy_nim_info( m_header );

            qDebug() << "extract data ";

            for ( int i = 0; i < blockSize; ++i )
            {
                b0data[i] = inputData[i];
            }

            dataVector->reserve( blockSize );

            for ( int z = 0; z < dimZ; ++z )
            {
                for ( int y = 0; y < dimY; ++y )
                {
                    for ( int x = 0; x < dimX; ++x )
                    {
                        ColumnVector v( dim );
                        for ( int j = 1; j < dim + 1; ++j )
                        {
                            v( j ) = inputData[j * blockSize + x + y * dimX + z * dimX * dimY];
                        }
                        dataVector->push_back( v );
                    }
                }
            }

            qDebug() << "extract data done";
            nifti_image_free( filedata );

            QString b0fn = m_fileName.path();
            if ( m_fileName.path().endsWith( ".nii.gz" ) )
            {
                b0fn.replace( ".nii.gz", "_b0.nii.gz" );
            }
            if ( m_fileName.path().endsWith( ".nii" ) )
            {
                b0fn.replace( ".nii.gz", "_b0.nii" );
            }

            DatasetScalar* datasetB0 = new DatasetScalar( b0fn, b0data, b0Hdr );
            m_dataset.push_back( datasetB0 );

            nifti_image* dsHdr = nifti_copy_nim_info( m_header );

            QVector<float> bvals2;
            QVector<QVector3D> bvecs;

            float* extData = reinterpret_cast<float*>( m_header->ext_list[0].edata );
            for ( int i = 0; i < dim; ++i )
            {
                bvals2.push_back( extData[i] );
            }

            for ( int i = 0; i < dim; ++i )
            {
                QVector3D v( extData[dim + 3 * i], extData[dim + 3 * i + 1], extData[dim + 3 * i + 2] );
                bvecs.push_back( v );
            }

            DatasetDWI* dataset = new DatasetDWI( m_fileName.path(), dataVector, b0data, bvals2, bvecs, dsHdr );
            m_dataset.push_back( dataset );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

QVector<float> Loader::loadBvals( QString fileName )
{
    QStringList slBvals;
    QVector<float> bvals;

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
            return bvals;
        }

        QTextStream in( &file );
        if ( !in.atEnd() )
        {
            slBvals = in.readLine().split( " ", QString::SkipEmptyParts );
            for ( int i = 0; i < slBvals.size(); ++i )
            {
                bool ok;
                //qDebug() << i << slBvals[i] << slBvals[i].toFloat( &ok );
                bvals.push_back( slBvals[i].toFloat( &ok ) );
                if ( !ok )
                {
                    qDebug() << "error while parsing bvals (" << i << "), conversion failure string to float";
                    bvals.clear();
                    return bvals;
                }

            }

        }
        return bvals;
    }
    else
    {
        qDebug() << "couldn't open " << fn;
        return bvals;
    }
}

QVector<QVector3D> Loader::loadBvecs( QString fileName, QVector<float> bvals )
{
    QString fn = m_fileName.path();
    fn.replace( ".nii.gz", ".bvec" );
    fn.replace( ".nii", ".bvec" );
    QDir dir2( fn );

    QVector<QVector3D> bvecs;

    if ( dir2.exists( dir2.absolutePath() ) )
    {
        QStringList slBvecs;

        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            qDebug() << "couldn't open " << fn;
            return bvecs;
        }

        QTextStream in( &file );
        while ( !in.atEnd() )
        {
            slBvecs.push_back( in.readLine() );
        }
        QString sX = slBvecs[0];
        QString sY = slBvecs[1];
        QString sZ = slBvecs[2];
        QStringList slX = sX.split( " ", QString::SkipEmptyParts );
        QStringList slY = sY.split( " ", QString::SkipEmptyParts );
        QStringList slZ = sZ.split( " ", QString::SkipEmptyParts );

        //qDebug() << "count bvals" << bvals.size() << bvals;
        if ( bvals.size() != slX.size() || bvals.size() != slY.size() || bvals.size() != slZ.size() )
        {
            qDebug() << "*** ERROR *** while loading dwi dataset, bvals don't match bvecs!";
            return bvecs;
        }

        for ( int i = 0; i < slX.size(); ++i )
        {
            if ( bvals[i] > 100 )
            {
                bool okX, okY, okZ;
                bvecs.push_back( QVector3D( slX[i].toDouble( &okX ), slY[i].toDouble( &okY ), slZ[i].toDouble( &okZ ) ) );
                if ( !( okX && okY && okZ ) )
                {
                    qDebug() << "error while parsing bvecs(" << i << "), conversion failure string to float";
                    bvecs.clear();
                    return bvecs;
                }
                //qDebug() << bvecs.last();
            }
        }
        return bvecs;
    }
    else
    {
        qDebug() << "couldn't open " << fn;
        return bvecs;
    }
}

bool Loader::loadMesh()
{
    return loadMeshBinary();
}

bool Loader::loadMeshBinary()
{
    /*
    qDebug() << "load mesh";
    QFile file( m_fileName.path() );
    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );
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
    float x, y, z;
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
*/
    return false;
}
