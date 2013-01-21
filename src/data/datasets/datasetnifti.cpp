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
    setProperty( FNPROP_DIM, m_header->ndim );
    setProperty( FNPROP_NX, m_header->nx );
    setProperty( FNPROP_NY, m_header->ny );
    setProperty( FNPROP_NZ, m_header->nz );

//    setProperty( FNPROP_NT, m_header->nt );
//    setProperty( FNPROP_NU, m_header->nu );
//    setProperty( FNPROP_NV, m_header->nv );
//    setProperty( FNPROP_NW, m_header->nw );
//    setProperty("dim", m_header->dim );
//    setProperty( FNPROP_NVOX, static_cast<int>( m_header->nvox ) );
//    setProperty( FNPROP_NBYPER, m_header->nbyper );
    setProperty( FNPROP_DATATYPE, m_header->datatype );
    setProperty( FNPROP_DX, m_header->dx );
    setProperty( FNPROP_DY, m_header->dy );
    setProperty( FNPROP_DZ, m_header->dz );
//    setProperty( FNPROP_DT, m_header->dt );
//    setProperty( FNPROP_DU, m_header->du );
//    setProperty( FNPROP_DV, m_header->dv );
//    setProperty( FNPROP_DW, m_header->dw );
//    setProperty( FNPROP_"pixdim", m_header->pixdim );
//    setProperty( FNPROP_"scl_slope", m_header->scl_slope );
//    setProperty( FNPROP_"scl_inter", m_header->scl_inter );
//    setProperty( FNPROP_"cal_min", m_header->cal_min );
//    setProperty( FNPROP_"cal_max", m_header->cal_max );
//    setProperty( FNPROP_"qform_code", m_header->qform_code );
//    setProperty( FNPROP_"sform_code", m_header->sform_code );
//    setProperty( FNPROP_"freq_dim", m_header->freq_dim );
//    setProperty( FNPROP_"phase_dim", m_header->phase_dim );
//    setProperty( FNPROP_"slice_dim", m_header->slice_dim );
//    setProperty( FNPROP_"slice_code", m_header->slice_code );
//    setProperty( FNPROP_"slice_start", m_header->slice_start );
//    setProperty( FNPROP_"slice_end", m_header->slice_end );
//    setProperty( FNPROP_"slice_duration", m_header->slice_duration );
//    setProperty( FNPROP_"quatern_b", m_header->quatern_b );
//    setProperty( FNPROP_"quatern_c", m_header->quatern_c );
//    setProperty( FNPROP_"quatern_d", m_header->quatern_d );
//    setProperty( FNPROP_"qoffset_x", m_header->qoffset_x );
//    setProperty( FNPROP_"qoffset_y", m_header->qoffset_y );
//    setProperty( FNPROP_"qoffset_z", m_header->qoffset_z );
//    setProperty( FNPROP_"qfac", m_header->qfac );
//    setProperty( FNPROP_qto_xyz", m_header->qto_xyz );
//    setProperty( FNPROP_qto_ijk", m_header->qto_ijk );
//    setProperty( FNPROP_ sto_xyz", m_header->sto_xyz );
//    setProperty( FNPROP_"sto_ijk", m_header->sto_ijk );
//    setProperty( FNPROP_"toffset", m_header->toffset );
//    setProperty( FNPROP_"xyz_units", m_header->xyz_units );
//    setProperty( FNPROP_"time_units", m_header->time_units );
//    setProperty( FNPROP_"nifti_type", m_header->nifti_type );
//    setProperty( FNPROP_"intent_code", m_header->intent_code );
//    setProperty( FNPROP_"intent_p1", m_header->intent_p1 );
//    setProperty( FNPROP_"intent_p2", m_header->intent_p2 );
//    setProperty( FNPROP_"intent_p3", m_header->intent_p3 );
//    setProperty( FNPROP_"intent_name", m_header->intent_name );
//    setProperty( FNPROP_"descrip", m_header->descrip );
//    setProperty( FNPROP_"aux_file", m_header->aux_file );
//    setProperty( FNPROP_"swapsize", m_header->swapsize );
//    setProperty( FNPROP_"byteorder", m_header->byteorder );

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
