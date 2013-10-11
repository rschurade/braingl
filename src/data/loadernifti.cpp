/*
 * loadernifti.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: schurade
 */

#include "loadernifti.h"

#include "datasets/datasetscalar.h"
#include "datasets/dataset3d.h"
#include "datasets/datasetbingham.h"
#include "datasets/datasetdwi.h"
#include "datasets/datasetfmri.h"
#include "datasets/datasettensor.h"
#include "datasets/datasetsh.h"

#include <QDebug>
#include <QDataStream>
#include <QVector3D>
#include <QtGui>


LoaderNifti::LoaderNifti( QDir fileName ) :
    m_fileName( fileName ),
    m_header( 0 ),
    m_datasetType( Fn::DatasetType::UNKNOWN )
{
}

LoaderNifti::~LoaderNifti()
{
}

QVector<Dataset*> LoaderNifti::getDataset()
{
    return m_dataset;
}

Fn::DatasetType LoaderNifti::getDatasetType()
{
    return m_datasetType;
}

bool LoaderNifti::load()
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
        m_datasetType = Fn::DatasetType::NIFTI_SCALAR;
        return loadNiftiScalar( fn );
    }
    else if ( m_header->dim[4] == 3 )
    {
        m_datasetType = Fn::DatasetType::NIFTI_VECTOR;
        return loadNiftiVector3D( fn );
    }
    else if ( m_header->dim[4] == 6 )
    {
        m_datasetType = Fn::DatasetType::NIFTI_TENSOR;
        return loadNiftiTensor( fn );
    }
    else if ( m_header->dim[4] == 27 )
    {
        m_datasetType = Fn::DatasetType::NIFTI_BINGHAM;
        return loadNiftiBingham( fn );
    }
    else if ( m_header->dim[4] == 15 || m_header->dim[4] == 28 || m_header->dim[4] == 45 )
    {
        m_datasetType = Fn::DatasetType::NIFTI_SH;
        return loadNiftiSH( fn );
    }
    else if ( m_header->dim[4] > 3 )
    {
        if ( QString( m_header->descrip ) == QString( "fnav2_dwi" ) )
        {
            qDebug() << "fnav2 dwi dataset found";
            m_datasetType = Fn::DatasetType::NIFTI_DWI;
            return loadNiftiDWI_FNAV2( fn );
        }
        else if ( m_header->datatype == 16 )
        {
            qDebug() << "fmri dataset found";
            m_datasetType = Fn::DatasetType::NIFTI_FMRI;
            return loadNiftiFMRI( fn );
        }
        else
        {
            m_datasetType = Fn::DatasetType::NIFTI_DWI;
            return loadNiftiDWI( fn );
        }
    }
    return false;
}

bool LoaderNifti::loadNiftiHeader( QString hdrPath )
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

bool LoaderNifti::loadNiftiScalar( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* inputData = reinterpret_cast<uint8_t*>( filedata->data );
            copyScalar( inputData );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* inputData = reinterpret_cast<int16_t*>( filedata->data );
            copyScalar( inputData );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* inputData = reinterpret_cast<int32_t*>( filedata->data );
            copyScalar( inputData );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );
            copyScalar( inputData );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* inputData = reinterpret_cast<int8_t*>( filedata->data );
            copyScalar( inputData );
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* inputData = reinterpret_cast<uint16_t*>( filedata->data );
            copyScalar( inputData );
            break;
        }
    }

    nifti_image_free( filedata );
    DatasetScalar* dataset = new DatasetScalar( fileName, m_scalarData, m_header );
    m_scalarData.clear();
    m_scalarData.squeeze();
    m_dataset.push_back( dataset );
    return true;
}

template<typename T> void LoaderNifti::copyScalar( T* inputData )
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];

    size_t blockSize = dimX * dimY * dimZ;
    m_scalarData.resize( blockSize );
    int index = 0;
    if ( isRadialogical() )
    {
        for ( int z = 0; z < dimZ; ++z )
        {
            for ( int y = 0; y < dimY; ++y )
            {
                for ( int x = dimX - 1; x >= 0; --x )
                {
                    m_scalarData[index++] = static_cast<float>( inputData[x + y * dimX + z * dimX * dimY] );
                }
            }
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
                    m_scalarData[index++] = static_cast<float>( inputData[x + y * dimX + z * dimX * dimY] );
                }
            }
        }
    }
}

bool LoaderNifti::loadNiftiVector3D( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* inputData = reinterpret_cast<uint8_t*>( filedata->data );
            copyVector( inputData );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* inputData = reinterpret_cast<int16_t*>( filedata->data );
            copyVector( inputData );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* inputData = reinterpret_cast<int32_t*>( filedata->data );
            copyVector( inputData );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );
            copyVector( inputData );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* inputData = reinterpret_cast<int8_t*>( filedata->data );
            copyVector( inputData );
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* inputData = reinterpret_cast<uint16_t*>( filedata->data );
            copyVector( inputData );
            break;
        }
    }

    nifti_image_free( filedata );
    Dataset3D* dataset = new Dataset3D( fileName, m_vectorData, m_header );
    m_vectorData.clear();
    m_vectorData.squeeze();
    m_dataset.push_back( dataset );
    return true;
}

template<typename T> void LoaderNifti::copyVector( T* inputData )
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;
    m_vectorData.resize( blockSize );
    int id = 0;
    int index = 0;
    if ( isRadialogical() )
    {
        for ( int z = 0; z < dimZ; ++z )
        {
            for ( int y = 0; y < dimY; ++y )
            {
                for ( int x = dimX - 1; x >= 0; --x )
                {
                    id = x + y * dimX + z * dimX * dimY;
                    m_vectorData[index].setX( inputData[id] );
                    m_vectorData[index].setY( inputData[blockSize + id] );
                    m_vectorData[index++].setZ( inputData[2 * blockSize + id] );
                }
            }
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
                    id = x + y * dimX + z * dimX * dimY;
                    m_vectorData[index].setX( inputData[id] );
                    m_vectorData[index].setY( inputData[blockSize + id] );
                    m_vectorData[index++].setZ( inputData[2 * blockSize + id] );
                }
            }
        }
    }
}

bool LoaderNifti::loadNiftiTensor( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];

    QVector<Matrix> dataVector;
    dataVector.reserve( blockSize );

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );

            if ( isRadialogical() )
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = dimX - 1; x >= 0; --x )
                        {
                            ColumnVector v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v( j + 1 ) = inputData[j * blockSize + id];
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

                            dataVector.push_back( m );
                        }
                    }
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
                            ColumnVector v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v( j + 1 ) = inputData[j * blockSize + id];
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

                            dataVector.push_back( m );
                        }
                    }
                }
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

bool LoaderNifti::loadNiftiSH( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;
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

    QVector<ColumnVector> dataVector;
    dataVector.reserve( blockSize );

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );

            if ( isRadialogical() )
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = dimX - 1; x >= 0; --x )
                        {
                            ColumnVector v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v( j + 1 ) = inputData[j * blockSize + id];
                            }
                            dataVector.push_back( v );
                        }
                    }
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
                            ColumnVector v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v( j + 1 ) = inputData[j * blockSize + id];
                            }
                            dataVector.push_back( v );
                        }
                    }
                }
            }
            nifti_image_free( filedata );

            DatasetSH* out = new DatasetSH( m_fileName.path(), dataVector, m_header );
            out->properties()->set( Fn::Property::D_CREATED_BY, (int) Fn::Algo::QBALL );
            out->properties()->set( Fn::Property::D_LOD, 2 );
            out->properties()->set( Fn::Property::D_ORDER, order );
            out->properties()->set( Fn::Property::D_RENDER_SLICE, 1 );

            m_dataset.push_back( out );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool LoaderNifti::loadNiftiBingham( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];

    QVector<QVector<float> > dataVector;
    dataVector.reserve( blockSize );

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );

            if ( isRadialogical() )
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = dimX - 1; x >= 0; --x )
                        {
                            QVector<float> v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v[ j ] = inputData[j * blockSize + id];
                            }
                            dataVector.push_back( v );
                        }
                    }
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
                            QVector<float> v( dim );
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int j = 0; j < dim; ++j )
                            {
                                v[ j ] = inputData[j * blockSize + id];
                            }
                            dataVector.push_back( v );
                        }
                    }
                }
            }


            nifti_image_free( filedata );

            DatasetBingham* out = new DatasetBingham( m_fileName.path(), dataVector, m_header );
            out->properties()->set( Fn::Property::D_CREATED_BY, (int) Fn::Algo::BINGHAM );
            out->properties()->set( Fn::Property::D_LOD, 2 );

            m_dataset.push_back( out );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool LoaderNifti::loadNiftiFMRI( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];
    qDebug() << "num images:" << dim;


    QVector<float> data( blockSize * dim );

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );

            int index = 0;

            if ( isRadialogical() )
            {

                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        for ( int x = dimX - 1; x >= 0; --x )
                        {
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int i = 0; i < dim; ++i )
                            {
                                data[i*blockSize+index] = inputData[i*blockSize+id];
                            }
                            index++;
                        }
                    }
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
                            int id = x + y * dimX + z * dimX * dimY;
                            for ( int i = 0; i < dim; ++i )
                            {
                                data[i*blockSize+index] = inputData[i*blockSize+id];
                            }
                            index++;
                        }
                    }
                }
            }
            nifti_image_free( filedata );

            nifti_image* dsHdr = nifti_copy_nim_info( m_header );
            DatasetFMRI* dataset = new DatasetFMRI( m_fileName.path(), data, dsHdr );
            m_dataset.push_back( dataset );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

bool LoaderNifti::loadNiftiDWI( QString fileName )
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

    QVector<ColumnVector> dataVector;

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_INT16:
        {
            QVector<float> b0data( blockSize );
            qDebug() << "block size: " << blockSize;
            int16_t* inputData;

            inputData = reinterpret_cast<int16_t*>( filedata->data );

            qDebug() << "extract data ";

            dataVector.reserve( blockSize );

            if ( isRadialogical() )
            {
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
                            dataVector.push_back( v );
                        }
                    }
                }

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
                            dataVector.push_back( v );
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

            nifti_image* b0Hdr = nifti_copy_nim_info( m_header );
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

bool LoaderNifti::loadNiftiDWI_FNAV2( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4] - 1;

    qDebug() << "num data:" << dim;

    QVector<ColumnVector> dataVector;

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

            dataVector.reserve( blockSize );

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
                        dataVector.push_back( v );
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

QVector<float> LoaderNifti::loadBvals( QString fileName )
{
    QStringList slBvals;
    QVector<float> bvals;

    QString fn = m_fileName.path();
    fn.replace( ".nii.gz", ".bval" );
    fn.replace( ".nii", ".bval" );
    QDir dir( fn );

    while ( !dir.exists( dir.absolutePath() ) )
    {
        QString path = dir.path();
        fn = QFileDialog::getOpenFileName( 0, "Select bval file", path );
        dir = QDir( fn );
    }

    if ( dir.exists( dir.absolutePath() ) )
    {
        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QMessageBox msgBox;
            msgBox.setText( "Error! Couldn't open" + fn );
            msgBox.exec();
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
                    QMessageBox msgBox;
                    msgBox.setText( "Error! While parsing bvals (" + QString::number( i ) + "), conversion failure string to float!" );
                    msgBox.exec();
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
        QMessageBox msgBox;
        msgBox.setText( "Error! Couldn't open" + fn );
        msgBox.exec();
        qDebug() << "couldn't open " << fn;
        return bvals;
    }
}

QVector<QVector3D> LoaderNifti::loadBvecs( QString fileName, QVector<float> bvals )
{
    QString fn = m_fileName.path();
    fn.replace( ".nii.gz", ".bvec" );
    fn.replace( ".nii", ".bvec" );
    QDir dir2( fn );

    QVector<QVector3D> bvecs;

    while ( !dir2.exists( dir2.absolutePath() ) )
    {
        QString path = dir2.path();
        fn = QFileDialog::getOpenFileName( 0, "Select bvec file", path );
        dir2 = QDir( fn );
    }

    if ( dir2.exists( dir2.absolutePath() ) )
    {
        QStringList slBvecs;

        QFile file( fn );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QMessageBox msgBox;
            msgBox.setText( "Error! Couldn't open" + fn );
            msgBox.exec();
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
            QMessageBox msgBox;
            msgBox.setText( "Error! While loading dwi dataset, bvals don't match bvecs!" );
            msgBox.exec();
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
                    QMessageBox msgBox;
                    msgBox.setText( "Error! While parsing bvecs(" + QString::number( i ) + "), conversion failure string to float" );
                    msgBox.exec();
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
        QMessageBox msgBox;
        msgBox.setText( "Error! Couldn't open" + fn );
        msgBox.exec();
        qDebug() << "couldn't open " << fn;
        return bvecs;
    }
}

bool LoaderNifti::isRadialogical()
{
    QMatrix4x4 qform;
    QMatrix4x4 sform;

    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j )
        {
            qform( i, j ) = m_header->qto_xyz.m[i][j];
        }
    }

    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j )
        {
            sform( i, j ) = m_header->sto_xyz.m[i][j];
        }
    }

    bool rad = false;

    if ( qform.determinant() < 0.0 && qform( 0, 0 ) < 0 )
    {
        qDebug() << m_fileName.path() << ": RADIOLOGICAL orientation in q-form detected. Flipping voxels on X-Axis";
        rad = true;
    }
    else
    {
        if ( sform.determinant() < 0.0 && sform( 0, 0 ) < 0 )
        {
            qDebug() << m_fileName.path() << ": RADIOLOGICAL orientation in s-form detected. Flipping voxels on X-Axis";
            rad = true;
        }
    }

    if ( rad )
    {
        m_header->qto_xyz.m[0][0] = fabs( m_header->qto_xyz.m[0][0] );
        m_header->sto_xyz.m[0][0] = fabs( m_header->sto_xyz.m[0][0] );

        float nx = m_header->dim[1] - 1;
        float sdx = m_header->sto_xyz.m[0][0];
        float qdx = m_header->qto_xyz.m[0][0];

        m_header->qto_xyz.m[0][3] -= nx * qdx;
        m_header->sto_xyz.m[0][3] -= nx * sdx;
    }
    return rad;
}
