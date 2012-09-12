/*
 * datasetnifti.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasetnifti.h"

DatasetNifti::DatasetNifti( QString filename, FN_DATASET_TYPE type, nifti_image* header ) :
    Dataset( filename, type ),
    m_header( header ),
    m_textureGLuint( 0 ),
    m_qform( 3, 3 ),
    m_sform( 3, 3 )
{
    parseNiftiHeader();
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

void DatasetNifti::parseNiftiHeader( )
{
    setProperty( "ndim", m_header->ndim );
    setProperty( "nx", m_header->nx );
    setProperty( "ny", m_header->ny );
    setProperty( "nz", m_header->nz );
    setProperty( "nt", m_header->nt );
    setProperty( "nu", m_header->nu );
    setProperty( "nv", m_header->nv );
    setProperty( "nw", m_header->nw );
    //setProperty("dim", m_header->dim );
    setProperty( "nvox", static_cast<int>( m_header->nvox ) );
    setProperty( "nbyper", m_header->nbyper );
    setProperty( "datatype", m_header->datatype );
    setProperty( "dx", m_header->dx );
    setProperty( "dy", m_header->dy );
    setProperty( "dz", m_header->dz );
    setProperty( "dt", m_header->dt );
    setProperty( "du", m_header->du );
    setProperty( "dv", m_header->dv );
    setProperty( "dw", m_header->dw );
    //setProperty("pixdim", m_header->pixdim );
    setProperty( "scl_slope", m_header->scl_slope );
    setProperty( "scl_inter", m_header->scl_inter );
    setProperty( "cal_min", m_header->cal_min );
    setProperty( "cal_max", m_header->cal_max );
    setProperty( "qform_code", m_header->qform_code );
    setProperty( "sform_code", m_header->sform_code );
    setProperty( "freq_dim", m_header->freq_dim );
    setProperty( "phase_dim", m_header->phase_dim );
    setProperty( "slice_dim", m_header->slice_dim );
    setProperty( "slice_code", m_header->slice_code );
    setProperty( "slice_start", m_header->slice_start );
    setProperty( "slice_end", m_header->slice_end );
    setProperty( "slice_duration", m_header->slice_duration );
    setProperty( "quatern_b", m_header->quatern_b );
    setProperty( "quatern_c", m_header->quatern_c );
    setProperty( "quatern_d", m_header->quatern_d );
    setProperty( "qoffset_x", m_header->qoffset_x );
    setProperty( "qoffset_y", m_header->qoffset_y );
    setProperty( "qoffset_z", m_header->qoffset_z );
    setProperty( "qfac", m_header->qfac );
    //setProperty(" qto_xyz", m_header->qto_xyz );
    //setProperty("qto_ijk", m_header->qto_ijk );
    //setProperty(" sto_xyz", m_header->sto_xyz );
    //setProperty("sto_ijk", m_header->sto_ijk );
    setProperty( "toffset", m_header->toffset );
    setProperty( "xyz_units", m_header->xyz_units );
    setProperty( "time_units", m_header->time_units );
    setProperty( "nifti_type", m_header->nifti_type );
    setProperty( "intent_code", m_header->intent_code );
    setProperty( "intent_p1", m_header->intent_p1 );
    setProperty( "intent_p2", m_header->intent_p2 );
    setProperty( "intent_p3", m_header->intent_p3 );
    //setProperty("intent_name", m_header->intent_name );
    //setProperty("descrip", m_header->descrip );
    //setProperty("aux_file", m_header->aux_file );
    setProperty( "swapsize", m_header->swapsize );
    setProperty( "byteorder", m_header->byteorder );

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_qform( i+1, j+1 ) =m_header->qto_xyz.m[i][j];
        }
    }

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            m_sform( i+1, j+1 ) =m_header->sto_xyz.m[i][j];
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
