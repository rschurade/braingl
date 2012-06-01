/*
 * datasetnifti.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "datasetnifti.h"

DatasetNifti::DatasetNifti( QString filename, FN_DATASET_TYPE type, void* data ) :
    Dataset( filename, type ),
    m_data( data ),
    m_textureGLuint( 0 )
{
}

DatasetNifti::~DatasetNifti()
{
    switch ( getProperty( "datatype").toInt() )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* data;
            data = reinterpret_cast< uint8_t* >( m_data );
            delete[] data;
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* data;
            data = reinterpret_cast< int16_t* >( m_data );
            delete[] data;
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* data;
            data = reinterpret_cast< int32_t* >( m_data );
            delete[] data;
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* data;
            data = reinterpret_cast< float* >( m_data );
            delete[] data;
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* data;
            data = reinterpret_cast< int8_t* >( m_data );
            delete[] data;
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* data;
            data = reinterpret_cast< uint16_t* >( m_data );
            delete[] data;
            break;
        }
    }
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
}

nifti_image* DatasetNifti::getHeader()
{
    return m_header;
}

void* DatasetNifti::getData()
{
    return m_data;
}
