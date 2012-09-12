/*
 * datasetnifti.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasetnifti.h"

DatasetNifti::DatasetNifti( QString filename, FN_DATASET_TYPE type ) :
    Dataset( filename, type ),
    m_textureGLuint( 0 ),
    m_qform( 3, 3 ),
    m_sform( 3, 3 )
{
}

DatasetNifti::~DatasetNifti()
{
}

GLuint DatasetNifti::getTextureGLuint()
{
    if ( m_textureGLuint == 0 )
    {
        createTexture();
    }
    return m_textureGLuint;
}

void DatasetNifti::parseNiftiHeader( nifti_image* header )
{
    m_header = header;

    setProperty( "ndim", header->ndim );
    setProperty( "nx", header->nx );
    setProperty( "ny", header->ny );
    setProperty( "nz", header->nz );
    setProperty( "nt", header->nt );
    setProperty( "nu", header->nu );
    setProperty( "nv", header->nv );
    setProperty( "nw", header->nw );
    //setProperty("dim", header->dim );
    setProperty( "nvox", static_cast<int>( header->nvox ) );
    setProperty( "nbyper", header->nbyper );
    setProperty( "datatype", header->datatype );
    setProperty( "dx", header->dx );
    setProperty( "dy", header->dy );
    setProperty( "dz", header->dz );
    setProperty( "dt", header->dt );
    setProperty( "du", header->du );
    setProperty( "dv", header->dv );
    setProperty( "dw", header->dw );
    //setProperty("pixdim", header->pixdim );
    setProperty( "scl_slope", header->scl_slope );
    setProperty( "scl_inter", header->scl_inter );
    setProperty( "cal_min", header->cal_min );
    setProperty( "cal_max", header->cal_max );
    setProperty( "qform_code", header->qform_code );
    setProperty( "sform_code", header->sform_code );
    setProperty( "freq_dim", header->freq_dim );
    setProperty( "phase_dim", header->phase_dim );
    setProperty( "slice_dim", header->slice_dim );
    setProperty( "slice_code", header->slice_code );
    setProperty( "slice_start", header->slice_start );
    setProperty( "slice_end", header->slice_end );
    setProperty( "slice_duration", header->slice_duration );
    setProperty( "quatern_b", header->quatern_b );
    setProperty( "quatern_c", header->quatern_c );
    setProperty( "quatern_d", header->quatern_d );
    setProperty( "qoffset_x", header->qoffset_x );
    setProperty( "qoffset_y", header->qoffset_y );
    setProperty( "qoffset_z", header->qoffset_z );
    setProperty( "qfac", header->qfac );
    //setProperty(" qto_xyz", header->qto_xyz );
    //setProperty("qto_ijk", header->qto_ijk );
    //setProperty(" sto_xyz", header->sto_xyz );
    //setProperty("sto_ijk", header->sto_ijk );
    setProperty( "toffset", header->toffset );
    setProperty( "xyz_units", header->xyz_units );
    setProperty( "time_units", header->time_units );
    setProperty( "nifti_type", header->nifti_type );
    setProperty( "intent_code", header->intent_code );
    setProperty( "intent_p1", header->intent_p1 );
    setProperty( "intent_p2", header->intent_p2 );
    setProperty( "intent_p3", header->intent_p3 );
    //setProperty("intent_name", header->intent_name );
    //setProperty("descrip", header->descrip );
    //setProperty("aux_file", header->aux_file );
    setProperty( "swapsize", header->swapsize );
    setProperty( "byteorder", header->byteorder );

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_qform( i+1, j+1 ) =header->qto_xyz.m[i][j];
        }
    }

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_sform( i+1, j+1 ) =header->sto_xyz.m[i][j];
        }
    }
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
