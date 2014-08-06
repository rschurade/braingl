/*
 * loadernifti.cpp
 *
 *  Created on: Oct 11, 2013
 *      Author: schurade
 */

#include "loadernifti.h"

#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/dataset3d.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasetdwi.h"
#include "../data/datasets/datasetfmri.h"
#include "../data/datasets/datasettensor.h"
#include "../data/datasets/datasetsh.h"
#include "../data/datasets/datasetisosurface.h"
#include "../data/datasets/datasetisoline.h"

#include <QDebug>
#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
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
    m_data.clear();
    m_dataset.clear();
}

std::vector<Dataset*> LoaderNifti::getDataset()
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

    if ( !loadData( fn ) )
    {
        return false;
    }

    if ( QString( m_header->descrip ) == QString( "fnav2_dwi" ) || QString( m_header->descrip ) == QString( "braingl_dwi" ) )
    {
        qDebug() << "braingl dwi dataset found";
        m_datasetType = Fn::DatasetType::NIFTI_DWI;
        return loadNiftiDWI_FNAV2( fn );
    }

    if( m_header->ext_list )
    {
        char* extData = reinterpret_cast<char*>( m_header->ext_list[0].edata );
        QByteArray ba( extData, m_header->ext_list[0].esize );

        QBuffer readBuffer( &ba );
        readBuffer.open( QIODevice::ReadOnly );
        QDataStream in( &readBuffer );

        in >> m_propStates;
    }

    if ( QString( m_header->descrip ) == QString( "braingl_isosurface" ) )
    {
        qDebug() << "braingl isosurface dataset found";
        m_datasetType = Fn::DatasetType::MESH_ISOSURFACE;
        return loadIsosurface();
    }

    if ( QString( m_header->descrip ) == QString( "braingl_isoline" ) )
    {
        qDebug() << "braingl isoline dataset found";
        m_datasetType = Fn::DatasetType::ISO_LINE;
        return loadIsoline();
    }

    int dim = m_header->dim[4];

    if ( dim > 3 && askTimeSeries( dim ) )
    {
        m_datasetType = Fn::DatasetType::NIFTI_FMRI;
        return loadNiftiFMRI();
    }

    switch ( dim )
    {
        case 0:
        case 1 :
            m_datasetType = Fn::DatasetType::NIFTI_SCALAR;
            return loadNiftiScalar();
            break;
        case 3:
        {
            m_datasetType = Fn::DatasetType::NIFTI_VECTOR;
            return loadNiftiVector3D();
        }
        case 6:
        {
            m_datasetType = Fn::DatasetType::NIFTI_TENSOR;
            return loadNiftiTensor();
        }
        case 27:
        {
            m_datasetType = Fn::DatasetType::NIFTI_BINGHAM;
            return loadNiftiBingham();
        }
        case 15:
        case 28:
        case 45:
        {
                m_datasetType = Fn::DatasetType::NIFTI_SH;
                return loadNiftiSH();
        }
        default:
        {
            m_datasetType = Fn::DatasetType::NIFTI_DWI;
            return loadNiftiDWI( fn );
        }
    }
    return false;
}

bool LoaderNifti::askTimeSeries( int dim )
{
    QMessageBox msgBox;
    msgBox.setText( "Found " + QString::number( dim ) + " images in nifti file." );
    msgBox.setInformativeText( "Interpret as fmri time series?" );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton( QMessageBox::Yes );
    int ret = msgBox.exec();
    if ( ret == QMessageBox::Yes )
    {
        return true;
    }
    else
    {
        return false;
    }
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

bool LoaderNifti::loadData( QString fileName )
{
    nifti_image* filedata = nifti_image_read( fileName.toStdString().c_str(), 1 );
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = qMax( 1, m_header->dim[4] );
    qDebug() << "num images:" << dim;

    try
    {
        m_data.resize( blockSize * dim );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    qDebug() << "start loading data";

    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* inputData = reinterpret_cast<uint8_t*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* inputData = reinterpret_cast<int16_t*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* inputData = reinterpret_cast<int32_t*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_UINT32:
        {
            uint32_t* inputData = reinterpret_cast<uint32_t*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* inputData = reinterpret_cast<float*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* inputData = reinterpret_cast<int8_t*>( filedata->data );
            copyData( inputData );
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* inputData = reinterpret_cast<uint16_t*>( filedata->data );
            copyData( inputData );
            break;
        }
    }
    nifti_image_free( filedata );
    return true;
}

template<typename T> void LoaderNifti::copyData( T* inputData )
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];

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
                        m_data[i*blockSize+index] = inputData[i*blockSize+id];
                    }
                    ++index;
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
                        m_data[i*blockSize+index] = inputData[i*blockSize+id];
                    }
                    ++index;
                }
            }
        }
    }
}


bool LoaderNifti::loadNiftiScalar()
{
    DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), m_data, m_header );
    m_data.clear();
    m_dataset.push_back( dataset );
    return true;
}

bool LoaderNifti::loadIsosurface()
{
    DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), m_data, m_header );
    m_data.clear();

    DatasetIsosurface* iso = new DatasetIsosurface( dataset );
    if ( m_propStates.size() > 0 )
    {
        iso->properties().setState( m_propStates );
    }
    m_dataset.push_back( iso );
    return true;
}

bool LoaderNifti::loadIsoline()
{
    DatasetScalar* dataset = new DatasetScalar( m_fileName.path(), m_data, m_header );
    m_data.clear();

    DatasetIsoline* iso = new DatasetIsoline( dataset );
    if ( m_propStates.size() > 0 )
    {
        iso->properties().setState( m_propStates );
    }
    m_dataset.push_back( iso );
    return true;
}

bool LoaderNifti::loadNiftiVector3D()
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;

    std::vector<QVector3D>vectorData( blockSize );

    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        vectorData[i].setX( m_data[i] );
        vectorData[i].setY( m_data[i + blockSize] );
        vectorData[i].setZ( m_data[i + 2 * blockSize] );
    }

    Dataset3D* dataset = new Dataset3D( m_fileName.path(), vectorData, m_header );
    m_data.clear();
    m_dataset.push_back( dataset );
    return true;
}

bool LoaderNifti::loadNiftiTensor()
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;

    std::vector<Matrix> dataVector;

    try
    {
        dataVector.reserve( blockSize );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        Matrix m( 3, 3 );
        m( 1, 1 ) = m_data[i];
        m( 1, 2 ) = m_data[i + blockSize];
        m( 1, 3 ) = m_data[i + 2 * blockSize];
        m( 2, 1 ) = m_data[i + blockSize];
        m( 2, 2 ) = m_data[i + 3 * blockSize];
        m( 2, 3 ) = m_data[i + 4 * blockSize];
        m( 3, 1 ) = m_data[i + 2 * blockSize];
        m( 3, 2 ) = m_data[i + 4 * blockSize];
        m( 3, 3 ) = m_data[i + 5 * blockSize];
        dataVector.push_back( m );
    }
    DatasetTensor* dataset = new DatasetTensor( m_fileName.path(), dataVector, m_header );
    m_dataset.push_back( dataset );
    m_data.clear();

    return true;
}

bool LoaderNifti::loadNiftiSH()
{
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

    std::vector<ColumnVector> dataVector;

    try
    {
        dataVector.reserve( blockSize );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        ColumnVector v( dim );

        for ( int j = 0; j < dim; ++j )
        {
            v( j + 1 ) = m_data[j * blockSize + i];
        }
        dataVector.push_back( v );
    }
    m_data.clear();

    DatasetSH* out = new DatasetSH( m_fileName.path(), dataVector, m_header );
    out->properties().set( Fn::Property::D_CREATED_BY, (int) Fn::Algo::QBALL );
    out->properties().set( Fn::Property::D_LOD, 2 );
    out->properties().set( Fn::Property::D_ORDER, order );
    out->properties().set( Fn::Property::D_RENDER_SLICE, 1 );

    m_dataset.push_back( out );
    return true;
}

bool LoaderNifti::loadNiftiBingham()
{
    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    size_t blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];

    std::vector<std::vector<float> > dataVector;

    try
    {
        dataVector.reserve( blockSize );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        std::vector<float> v( dim );

        for ( int j = 0; j < dim; ++j )
        {
            v[j] = m_data[j * blockSize + i];
        }
        dataVector.push_back( v );
    }
    m_data.clear();

    DatasetBingham* out = new DatasetBingham( m_fileName.path(), dataVector, m_header );
    out->properties().set( Fn::Property::D_CREATED_BY, (int) Fn::Algo::BINGHAM );
    out->properties().set( Fn::Property::D_LOD, 2 );

    m_dataset.push_back( out );

    return true;
}

bool LoaderNifti::loadNiftiFMRI()
{
    nifti_image* dsHdr = nifti_copy_nim_info( m_header );
    DatasetFMRI* dataset = new DatasetFMRI( m_fileName.path(), m_data, dsHdr );
    m_dataset.push_back( dataset );
    m_data.clear();
    return true;
}

bool LoaderNifti::loadNiftiDWI( QString fileName )
{
    std::vector<float> bvals = loadBvals( fileName );

    if ( (int)bvals.size() == 0 || (int)bvals.size() != m_header->dim[4] )
    {
        qDebug() << "*** ERROR *** while loading dwi dataset, count bvals doesn't match nifti image dim!";
        return false;
    }
    int numB0 = 0;
    std::vector<float> bvals2;
    for ( unsigned int i = 0; i < bvals.size(); ++i )
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

    std::vector<QVector3D> bvecs = loadBvecs( fileName, bvals );
    if ( bvecs.size() == 0 )
    {
        qDebug() << "*** ERROR *** while loading bvecs!";
        return false;
    }

    int dimX = m_header->dim[1];
    int dimY = m_header->dim[2];
    int dimZ = m_header->dim[3];
    unsigned int blockSize = dimX * dimY * dimZ;
    int dim = m_header->dim[4];
    int numData = dim - numB0;
    qDebug() << "num data:" << numData;

    if ( numData > dim || bvals2.size() != bvecs.size() )
    {
        qDebug() << "*** ERROR *** parsing bval and bvec files!";
        return false;
    }

    std::vector<ColumnVector> dataVector;

    try
    {
        dataVector.reserve( blockSize );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    std::vector<float> b0data( blockSize );
    for ( unsigned int i = 0; i < blockSize; ++i )
    {
        ColumnVector v( numData );

        int dataIndex = 1;
        for ( int j = 0; j < dim; ++j )
        {
            if ( bvals[j] > 100 )
            {
                v( dataIndex ) = m_data[j * blockSize + i];
                ++dataIndex;
            }
            else
            {
                b0data[i] += m_data[j * blockSize + i] / numB0;
            }
        }
        dataVector.push_back( v );
    }

    QString b0fn = m_fileName.path();
    if ( m_fileName.path().endsWith( ".nii.gz" ) )
    {
        b0fn.replace( ".nii.gz", "_b0.nii.gz" );
    }
    if ( m_fileName.path().endsWith( ".nii" ) )
    {
        b0fn.replace( ".nii.gz", "_b0.nii" );
    }
    m_data.clear();

    nifti_image* b0Hdr = nifti_copy_nim_info( m_header );
    DatasetScalar* datasetB0 = new DatasetScalar( b0fn, b0data, b0Hdr );
    m_dataset.push_back( datasetB0 );

    nifti_image* dsHdr = nifti_copy_nim_info( m_header );
    DatasetDWI* dataset = new DatasetDWI( m_fileName.path(), dataVector, b0data, bvals2, bvecs, dsHdr );
    m_dataset.push_back( dataset );

    return true;
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

    std::vector<ColumnVector> dataVector;

    try
    {
        dataVector.reserve( blockSize );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "*** error *** failed to allocate memory for dataset";
        return false;
    }

    qDebug() << "start loading data";
    switch ( m_header->datatype )
    {
        case NIFTI_TYPE_FLOAT32:
        {
            std::vector<float> b0data( blockSize );
            qDebug() << "block size: " << blockSize;
            float* inputData;

            inputData = reinterpret_cast<float*>( filedata->data );

            nifti_image* b0Hdr = nifti_copy_nim_info( m_header );

            qDebug() << "extract data ";

            for ( int i = 0; i < blockSize; ++i )
            {
                b0data[i] = inputData[i];
            }

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

            std::vector<float> bvals2;
            std::vector<QVector3D> bvecs;

            float* extData;
            extData = reinterpret_cast<float*>( m_header->ext_list[0].edata );
            int extSize = m_header->ext_list[0].esize;
            qDebug() << extSize;

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
            if ( m_propStates.size() > 0 )
            {
                dataset->properties().setState( m_propStates );
            }
            m_dataset.push_back( dataset );

            qDebug() << "end loading data";
            return true;
            break;
        }
    }
    return false;
}

std::vector<float> LoaderNifti::loadBvals( QString fileName )
{
    QStringList slBvals;
    std::vector<float> bvals;

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

std::vector<QVector3D> LoaderNifti::loadBvecs( QString fileName, std::vector<float> bvals )
{
    QString fn = m_fileName.path();
    fn.replace( ".nii.gz", ".bvec" );
    fn.replace( ".nii", ".bvec" );
    QDir dir2( fn );

    std::vector<QVector3D> bvecs;

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
        if ( (int)bvals.size() != slX.size() || (int)bvals.size() != slY.size() || (int)bvals.size() != slZ.size() )
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
