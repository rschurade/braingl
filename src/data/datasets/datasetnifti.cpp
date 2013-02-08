/*
 * datasetnifti.cpp
 *
 *  Created on: May 9, 2012
 *      Author: Ralph Schurade
 */
#include "datasetnifti.h"

#include <QtCore/QDebug>

DatasetNifti::DatasetNifti( QString filename, Fn::DatasetType type, nifti_image* header ) :
        Dataset( filename, type ), m_header( header ), m_qform( 3, 3 ), m_sform( 3, 3 )
{
    parseNiftiHeader();
}

DatasetNifti::~DatasetNifti()
{
}

void DatasetNifti::parseNiftiHeader()
{
    m_properties.set( Fn::Property::DIM, m_header->ndim );
    m_properties.set( Fn::Property::NX, m_header->nx );
    m_properties.set( Fn::Property::NY, m_header->ny );
    m_properties.set( Fn::Property::NZ, m_header->nz );

//    m_properties.set( Fn::Property::NT, m_header->nt );
//    m_properties.set( Fn::Property::NU, m_header->nu );
//    m_properties.set( Fn::Property::NV, m_header->nv );
//    m_properties.set( Fn::Property::NW, m_header->nw );
//    m_properties.set("dim", m_header->dim );
//    m_properties.set( Fn::Property::NVOX, static_cast<int>( m_header->nvox ) );
//    m_properties.set( Fn::Property::NBYPER, m_header->nbyper );
    m_properties.set( Fn::Property::DATATYPE, m_header->datatype );
    m_properties.set( Fn::Property::DX, m_header->dx );
    m_properties.set( Fn::Property::DY, m_header->dy );
    m_properties.set( Fn::Property::DZ, m_header->dz );
//    m_properties.set( Fn::Property::DT, m_header->dt );
//    m_properties.set( Fn::Property::DU, m_header->du );
//    m_properties.set( Fn::Property::DV, m_header->dv );
//    m_properties.set( Fn::Property::DW, m_header->dw );
//    m_properties.set( Fn::Property::"pixdim", m_header->pixdim );
//    m_properties.set( Fn::Property::"scl_slope", m_header->scl_slope );
//    m_properties.set( Fn::Property::"scl_inter", m_header->scl_inter );
//    m_properties.set( Fn::Property::"cal_min", m_header->cal_min );
//    m_properties.set( Fn::Property::"cal_max", m_header->cal_max );
//    m_properties.set( Fn::Property::"qform_code", m_header->qform_code );
//    m_properties.set( Fn::Property::"sform_code", m_header->sform_code );
//    m_properties.set( Fn::Property::"freq_dim", m_header->freq_dim );
//    m_properties.set( Fn::Property::"phase_dim", m_header->phase_dim );
//    m_properties.set( Fn::Property::"slice_dim", m_header->slice_dim );
//    m_properties.set( Fn::Property::"slice_code", m_header->slice_code );
//    m_properties.set( Fn::Property::"slice_start", m_header->slice_start );
//    m_properties.set( Fn::Property::"slice_end", m_header->slice_end );
//    m_properties.set( Fn::Property::"slice_duration", m_header->slice_duration );
//    m_properties.set( Fn::Property::"quatern_b", m_header->quatern_b );
//    m_properties.set( Fn::Property::"quatern_c", m_header->quatern_c );
//    m_properties.set( Fn::Property::"quatern_d", m_header->quatern_d );
//    m_properties.set( Fn::Property::"qoffset_x", m_header->qoffset_x );
//    m_properties.set( Fn::Property::"qoffset_y", m_header->qoffset_y );
//    m_properties.set( Fn::Property::"qoffset_z", m_header->qoffset_z );
//    m_properties.set( Fn::Property::"qfac", m_header->qfac );
//    m_properties.set( Fn::Property::qto_xyz", m_header->qto_xyz );
//    m_properties.set( Fn::Property::qto_ijk", m_header->qto_ijk );
//    m_properties.set( Fn::Property:: sto_xyz", m_header->sto_xyz );
//    m_properties.set( Fn::Property::"sto_ijk", m_header->sto_ijk );
//    m_properties.set( Fn::Property::"toffset", m_header->toffset );
//    m_properties.set( Fn::Property::"xyz_units", m_header->xyz_units );
//    m_properties.set( Fn::Property::"time_units", m_header->time_units );
//    m_properties.set( Fn::Property::"nifti_type", m_header->nifti_type );
//    m_properties.set( Fn::Property::"intent_code", m_header->intent_code );
//    m_properties.set( Fn::Property::"intent_p1", m_header->intent_p1 );
//    m_properties.set( Fn::Property::"intent_p2", m_header->intent_p2 );
//    m_properties.set( Fn::Property::"intent_p3", m_header->intent_p3 );
//    m_properties.set( Fn::Property::"intent_name", m_header->intent_name );
//    m_properties.set( Fn::Property::"descrip", m_header->descrip );
//    m_properties.set( Fn::Property::"aux_file", m_header->aux_file );
//    m_properties.set( Fn::Property::"swapsize", m_header->swapsize );
//    m_properties.set( Fn::Property::"byteorder", m_header->byteorder );

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

QString DatasetNifti::getNiftiDataType( const int type )
{
    switch ( type )
    {
        case 0:
            return QString( "unknown" );
            break;
        case 1:
            return QString( "binary" );
            break;
        case 2:
            return QString( "unsigned char" );
            break;
        case 4:
            return QString( "signed short" );
            break;
        case 8:
            return QString( "signed int" );
            break;
        case 16:
            return QString( "float" );
            break;
        case 32:
            return QString( "complex" );
            break;
        case 64:
            return QString( "double" );
            break;
        case 128:
            return QString( "RGB" );
            break;
        default:
            return QString( "unknown" );
            break;
    }
    return QString( "unknown" );
}
