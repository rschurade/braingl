/*
 * writer.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasets/datasetnifti.h"
#include "datasets/datasetscalar.h"

#include "writer.h"

Writer::Writer( Dataset* dataset, QString fileName ) :
    m_dataset( dataset ),
    m_fileName( fileName )
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
            nifti_image* out = createHeader();

            QVector<float> data = dynamic_cast<DatasetScalar*>( m_dataset )->getData();

            out->data = &(data[0]);
            if( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
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
            /*
            qDebug() << "writing nifti file";
            DatasetNifti* dsNifti = dynamic_cast<DatasetNifti*>( m_dataset );
            nifti_image* out =  nifti_copy_nim_info( dsNifti->getHeader() );
            out->data = dsNifti->getData();
            if( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem";
            }

            nifti_image_write( out );
            break;
            */
        }
        break;
    }
    return true;
}

nifti_image* Writer::createHeader()
{
    nifti_image* img = dynamic_cast<DatasetNifti*>( m_dataset )->getHeader();

    nifti_image* out = nifti_simple_init_nim();

    out->nx = img->nx;
    out->ny = img->ny;
    out->nz = img->nz;

    out->nvox = img->nvox;

    out->dx = img->dx;
    out->dy = img->dy;
    out->dz = img->dz;

    out->nifti_type = 1; // 1==NIFTI-1 (1 file)

    out->freq_dim = 1;
    out->phase_dim = 2;
    out->slice_dim = 3;

    out->qform_code = 1;
    out->sform_code = 1;

    out->nt = img->nt;
    out->nv = 4;
    out->ndim = 4;

    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            out->qto_xyz.m[i][j] = img->qto_xyz.m[i][j];
            out->sto_xyz.m[i][j] = img->sto_xyz.m[i][j];
        }
    }

    {
        float dx, dy, dz;
        nifti_mat44_to_quatern( out->qto_xyz, &( out->quatern_b ),
                                &( out->quatern_c ), &( out->quatern_d ),
                                &( out->qoffset_x ), &( out->qoffset_y ),
                                &( out->qoffset_z ),
                                &dx, &dy, &dz,
                                &( out->qfac ) );
    }

    out->qto_ijk = nifti_mat44_inverse( out->qto_xyz );
    out->sto_ijk = nifti_mat44_inverse( out->sto_xyz );

    out->nbyper = 4;
    out->datatype = DT_FLOAT;

    return out;
}
