/*
 * writer.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasets/dataset3d.h"
#include "datasets/datasetnifti.h"
#include "datasets/datasetdwi.h"
#include "datasets/datasetscalar.h"
#include "datasets/datasettensor.h"
#include "datasets/datasetqball.h"

#include "writer.h"

Writer::Writer( Dataset* dataset, QString fileName ) :
        m_dataset( dataset ), m_fileName( fileName )
{
}

Writer::~Writer()
{
}

bool Writer::save()
{
    switch ( m_dataset->getProperty( "type" ).toInt() )
    {
        case FNDT_NIFTI_SCALAR:
        {
            nifti_image* out = createHeader( 1 );

            QVector<float> data = dynamic_cast<DatasetScalar*>( m_dataset )->getData();

            out->data = &( data[0] );
            if ( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case FNDT_NIFTI_VECTOR:
        {
            QVector<QVector3D>* data = dynamic_cast<Dataset3D*>( m_dataset )->getData();

            nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();

            int dim = 3;
            nifti_image* out = createHeader( dim );
            QVector<float> outData( img->nx * img->ny * img->nz * dim );

            int blockSize = img->nx * img->ny * img->nz;
            setDescrip( out, "fnav2_vec3" );

            for ( int z = 0; z < img->nz; ++z )
            {
                for ( int y = 0; y < img->ny; ++y )
                {
                    for ( int x = 0; x < img->nx; ++x )
                    {
                        QVector3D vData = data->at( x + y * img->nx + z * img->nx * img->ny );

                        outData[( x + y * img->nx + z * img->nx * img->ny )] = vData.x();
                        outData[( x + y * img->nx + z * img->nx * img->ny + blockSize )] = vData.y();
                        outData[( x + y * img->nx + z * img->nx * img->ny + 2 * blockSize )] = vData.z();
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case FNDT_NIFTI_TENSOR:
        {
            nifti_image* out = createHeader( 6 );

            QVector<Matrix>* data = dynamic_cast<DatasetTensor*>( m_dataset )->getData();

            nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();
            QVector<float> outData( img->nx * img->ny * img->nz * 6 );

            int blockSize = img->nx * img->ny * img->nz;
            setDescrip( out, "fnav2_tensor" );

            for ( int z = 0; z < img->nz; ++z )
            {
                for ( int y = 0; y < img->ny; ++y )
                {
                    for ( int x = 0; x < img->nx; ++x )
                    {
                        Matrix tensor = data->at( x + y * img->nx + z * img->nx * img->ny );
                        outData[( x + y * img->nx + z * img->nx * img->ny )] = tensor( 1, 1 );
                        outData[( x + y * img->nx + z * img->nx * img->ny ) + blockSize] = tensor( 1, 2 );
                        outData[( x + y * img->nx + z * img->nx * img->ny ) + blockSize * 2] = tensor( 1, 3 );
                        outData[( x + y * img->nx + z * img->nx * img->ny ) + blockSize * 3] = tensor( 2, 2 );
                        outData[( x + y * img->nx + z * img->nx * img->ny ) + blockSize * 4] = tensor( 2, 3 );
                        outData[( x + y * img->nx + z * img->nx * img->ny ) + blockSize * 5] = tensor( 3, 3 );
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case FNDT_NIFTI_QBALL:
        {
            QVector<ColumnVector>* data = dynamic_cast<DatasetQBall*>( m_dataset )->getData();

            nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();

            int dim = data->at( 0 ).Nrows();
            nifti_image* out = createHeader( dim );
            QVector<float> outData( img->nx * img->ny * img->nz * dim );

            int blockSize = img->nx * img->ny * img->nz;
            setDescrip( out, "fnav2_qball" );

            for ( int z = 0; z < img->nz; ++z )
            {
                for ( int y = 0; y < img->ny; ++y )
                {
                    for ( int x = 0; x < img->nx; ++x )
                    {
                        ColumnVector vData = data->at( x + y * img->nx + z * img->nx * img->ny );

                        for ( int i = 0; i < dim; ++i )
                        {
                            outData[( x + y * img->nx + z * img->nx * img->ny + i * blockSize )] = vData( i + 1 );
                        }
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case FNDT_NIFTI_DWI:
        {
            QVector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( m_dataset )->getData();
            QVector<float> b0data = dynamic_cast<DatasetDWI*>( m_dataset )->getB0Data();

            nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();

            int dim = data->at( 0 ).Nrows() + 1;
            nifti_image* out = createHeader( dim );
            QVector<float> outData( img->nx * img->ny * img->nz * dim );

            QVector<float> bvals = dynamic_cast<DatasetDWI*>( m_dataset )->getBvals();
            QVector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( m_dataset )->getBvecs();
            QVector<float> bvalOut;
            for ( int i = 0; i < bvals.size(); ++i )
            {
                bvalOut.push_back( bvals[i] );
            }
            for ( int i = 0; i < bvecs.size(); ++i )
            {
                bvalOut.push_back( bvecs[i].x() );
                bvalOut.push_back( bvecs[i].y() );
                bvalOut.push_back( bvecs[i].z() );
            }
            char* extData = reinterpret_cast<char*>( &bvalOut[0] );
            nifti_add_extension( out, extData, bvalOut.size() * 4, 0 );

            int blockSize = img->nx * img->ny * img->nz;
            setDescrip( out, "fnav2_dwi" );

            for ( int i = 0; i < blockSize; ++i )
            {
                outData[i] = b0data[i];
            }

            for ( int z = 0; z < img->nz; ++z )
            {
                for ( int y = 0; y < img->ny; ++y )
                {
                    for ( int x = 0; x < img->nx; ++x )
                    {
                        ColumnVector vData = data->at( x + y * img->nx + z * img->nx * img->ny );
                        for ( int i = 1; i < dim; ++i )
                        {
                            outData[( x + y * img->nx + z * img->nx * img->ny + i * blockSize )] = vData( i );
                        }
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
    }
    return true;
}

nifti_image* Writer::createHeader( int dim )
{
    nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();

    nifti_image* out = nifti_simple_init_nim();

    out->nx = img->nx;
    out->ny = img->ny;
    out->nz = img->nz;

    out->nvox = img->nx * img->ny * img->nz * dim;

    out->dx = img->dx;
    out->dy = img->dy;
    out->dz = img->dz;

    out->nifti_type = 1; // 1==NIFTI-1 (1 file)

    out->freq_dim = 1;
    out->phase_dim = 2;
    out->slice_dim = 3;

    out->qform_code = 1;
    out->sform_code = 1;

    out->nt = dim;
    out->nv = 4;
    out->ndim = 4;

    for ( size_t i = 0; i < 4; ++i )
    {
        for ( size_t j = 0; j < 4; ++j )
        {
            out->qto_xyz.m[i][j] = img->qto_xyz.m[i][j];
            out->sto_xyz.m[i][j] = img->sto_xyz.m[i][j];
        }
    }

    {
        float dx, dy, dz;
        nifti_mat44_to_quatern( out->qto_xyz, &( out->quatern_b ), &( out->quatern_c ), &( out->quatern_d ), &( out->qoffset_x ), &( out->qoffset_y ),
                &( out->qoffset_z ), &dx, &dy, &dz, &( out->qfac ) );
    }

    out->qto_ijk = nifti_mat44_inverse( out->qto_xyz );
    out->sto_ijk = nifti_mat44_inverse( out->sto_xyz );

    out->nbyper = 4;
    out->datatype = DT_FLOAT;

    return out;
}

void Writer::setDescrip( nifti_image* hdr, QString descrip )
{
    if ( descrip.length() > 80 )
    {
        qDebug() << "Writer: descrip too long ";
        return;
    }
    for ( int i = 0; i < 80; ++i )
    {
        hdr->descrip[i] = 0;
    }
    for ( int i = 0; i < descrip.length(); ++i )
    {
        hdr->descrip[i] = descrip.at( i ).toLatin1();
    }
}
