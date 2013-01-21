/*
 * datasetnifti.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasetnifti.h"

DatasetNifti::DatasetNifti( QString filename, FN_DATASET_TYPE type, nifti_image* header ) :
        Dataset( filename, type ), m_header( header ), m_qform( 3, 3 ), m_sform( 3, 3 )
{
    parseNiftiHeader();
}

DatasetNifti::~DatasetNifti()
{
}

void DatasetNifti::parseNiftiHeader()
{
    m_properties.set( FNPROP_DIM, m_header->ndim );
    m_properties.set( FNPROP_NX, m_header->nx );
    m_properties.set( FNPROP_NY, m_header->ny );
    m_properties.set( FNPROP_NZ, m_header->nz );

//    m_properties.set( FNPROP_NT, m_header->nt );
//    m_properties.set( FNPROP_NU, m_header->nu );
//    m_properties.set( FNPROP_NV, m_header->nv );
//    m_properties.set( FNPROP_NW, m_header->nw );
//    m_properties.set("dim", m_header->dim );
//    m_properties.set( FNPROP_NVOX, static_cast<int>( m_header->nvox ) );
//    m_properties.set( FNPROP_NBYPER, m_header->nbyper );
    m_properties.set( FNPROP_DATATYPE, m_header->datatype );
    m_properties.set( FNPROP_DX, m_header->dx );
    m_properties.set( FNPROP_DY, m_header->dy );
    m_properties.set( FNPROP_DZ, m_header->dz );
//    m_properties.set( FNPROP_DT, m_header->dt );
//    m_properties.set( FNPROP_DU, m_header->du );
//    m_properties.set( FNPROP_DV, m_header->dv );
//    m_properties.set( FNPROP_DW, m_header->dw );
//    m_properties.set( FNPROP_"pixdim", m_header->pixdim );
//    m_properties.set( FNPROP_"scl_slope", m_header->scl_slope );
//    m_properties.set( FNPROP_"scl_inter", m_header->scl_inter );
//    m_properties.set( FNPROP_"cal_min", m_header->cal_min );
//    m_properties.set( FNPROP_"cal_max", m_header->cal_max );
//    m_properties.set( FNPROP_"qform_code", m_header->qform_code );
//    m_properties.set( FNPROP_"sform_code", m_header->sform_code );
//    m_properties.set( FNPROP_"freq_dim", m_header->freq_dim );
//    m_properties.set( FNPROP_"phase_dim", m_header->phase_dim );
//    m_properties.set( FNPROP_"slice_dim", m_header->slice_dim );
//    m_properties.set( FNPROP_"slice_code", m_header->slice_code );
//    m_properties.set( FNPROP_"slice_start", m_header->slice_start );
//    m_properties.set( FNPROP_"slice_end", m_header->slice_end );
//    m_properties.set( FNPROP_"slice_duration", m_header->slice_duration );
//    m_properties.set( FNPROP_"quatern_b", m_header->quatern_b );
//    m_properties.set( FNPROP_"quatern_c", m_header->quatern_c );
//    m_properties.set( FNPROP_"quatern_d", m_header->quatern_d );
//    m_properties.set( FNPROP_"qoffset_x", m_header->qoffset_x );
//    m_properties.set( FNPROP_"qoffset_y", m_header->qoffset_y );
//    m_properties.set( FNPROP_"qoffset_z", m_header->qoffset_z );
//    m_properties.set( FNPROP_"qfac", m_header->qfac );
//    m_properties.set( FNPROP_qto_xyz", m_header->qto_xyz );
//    m_properties.set( FNPROP_qto_ijk", m_header->qto_ijk );
//    m_properties.set( FNPROP_ sto_xyz", m_header->sto_xyz );
//    m_properties.set( FNPROP_"sto_ijk", m_header->sto_ijk );
//    m_properties.set( FNPROP_"toffset", m_header->toffset );
//    m_properties.set( FNPROP_"xyz_units", m_header->xyz_units );
//    m_properties.set( FNPROP_"time_units", m_header->time_units );
//    m_properties.set( FNPROP_"nifti_type", m_header->nifti_type );
//    m_properties.set( FNPROP_"intent_code", m_header->intent_code );
//    m_properties.set( FNPROP_"intent_p1", m_header->intent_p1 );
//    m_properties.set( FNPROP_"intent_p2", m_header->intent_p2 );
//    m_properties.set( FNPROP_"intent_p3", m_header->intent_p3 );
//    m_properties.set( FNPROP_"intent_name", m_header->intent_name );
//    m_properties.set( FNPROP_"descrip", m_header->descrip );
//    m_properties.set( FNPROP_"aux_file", m_header->aux_file );
//    m_properties.set( FNPROP_"swapsize", m_header->swapsize );
//    m_properties.set( FNPROP_"byteorder", m_header->byteorder );

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_qform( i + 1, j + 1 ) = m_header->qto_xyz.m[i][j];
        }
    }

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_sform( i + 1, j + 1 ) = m_header->sto_xyz.m[i][j];
        }
    }

    m_header->descrip[0] = 'f';
    m_header->descrip[1] = 'n';
    m_header->descrip[2] = 'a';
    m_header->descrip[3] = 'v';
    m_header->descrip[4] = '2';
    m_header->descrip[5] = 0;

}

nifti_image* DatasetNifti::getHeader()
{
    return m_header;
}

Matrix DatasetNifti::getQForm()
{
    return m_qform;
}

Matrix DatasetNifti::getSForm()
{
    return m_sform;
}
