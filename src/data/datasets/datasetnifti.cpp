/*
 * datasetnifti.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "datasetnifti.h"

#include "../../gui/gl/colormapfunctions.h"

#include <QDebug>

DatasetNifti::DatasetNifti( QDir filename, Fn::DatasetType type, nifti_image* header ) :
    Dataset( filename, type ),
    m_header( header ),
    m_qform( 3, 3 ),
    m_sform( 3, 3 )
{
    parseNiftiHeader();
}

DatasetNifti::~DatasetNifti()
{
}

void DatasetNifti::parseNiftiHeader()
{
    m_properties["maingl"]->create( Fn::Property::D_DIM, m_header->dim[4] );
    m_properties["maingl"]->create( Fn::Property::D_NX, m_header->nx );
    m_properties["maingl"]->create( Fn::Property::D_NY, m_header->ny );
    m_properties["maingl"]->create( Fn::Property::D_NZ, m_header->nz );

//    m_properties["maingl"]->create( Fn::Property::NT, m_header->nt );
//    m_properties["maingl"]->create( Fn::Property::NU, m_header->nu );
//    m_properties["maingl"]->create( Fn::Property::NV, m_header->nv );
//    m_properties["maingl"]->create( Fn::Property::NW, m_header->nw );
//    m_properties["maingl"]->create("dim", m_header->dim );
//    m_properties["maingl"]->create( Fn::Property::NVOX, static_cast<int>( m_header->nvox ) );
//    m_properties["maingl"]->create( Fn::Property::NBYPER, m_header->nbyper );
    m_properties["maingl"]->create( Fn::Property::D_DATATYPE, m_header->datatype );
    m_properties["maingl"]->create( Fn::Property::D_DX, m_header->dx );
    m_properties["maingl"]->create( Fn::Property::D_DY, m_header->dy );
    m_properties["maingl"]->create( Fn::Property::D_DZ, m_header->dz );
//    m_properties["maingl"]->create( Fn::Property::DT, m_header->dt );
//    m_properties["maingl"]->create( Fn::Property::DU, m_header->du );
//    m_properties["maingl"]->create( Fn::Property::DV, m_header->dv );
//    m_properties["maingl"]->create( Fn::Property::DW, m_header->dw );
//    m_properties["maingl"]->create( Fn::Property::"pixdim", m_header->pixdim );
//    m_properties["maingl"]->create( Fn::Property::"scl_slope", m_header->scl_slope );
//    m_properties["maingl"]->create( Fn::Property::"scl_inter", m_header->scl_inter );
//    m_properties["maingl"]->create( Fn::Property::"cal_min", m_header->cal_min );
//    m_properties["maingl"]->create( Fn::Property::"cal_max", m_header->cal_max );
//    m_properties["maingl"]->create( Fn::Property::"qform_code", m_header->qform_code );
//    m_properties["maingl"]->create( Fn::Property::"sform_code", m_header->sform_code );
//    m_properties["maingl"]->create( Fn::Property::"freq_dim", m_header->freq_dim );
//    m_properties["maingl"]->create( Fn::Property::"phase_dim", m_header->phase_dim );
//    m_properties["maingl"]->create( Fn::Property::"slice_dim", m_header->slice_dim );
//    m_properties["maingl"]->create( Fn::Property::"slice_code", m_header->slice_code );
//    m_properties["maingl"]->create( Fn::Property::"slice_start", m_header->slice_start );
//    m_properties["maingl"]->create( Fn::Property::"slice_end", m_header->slice_end );
//    m_properties["maingl"]->create( Fn::Property::"slice_duration", m_header->slice_duration );
//    m_properties["maingl"]->create( Fn::Property::"quatern_b", m_header->quatern_b );
//    m_properties["maingl"]->create( Fn::Property::"quatern_c", m_header->quatern_c );
//    m_properties["maingl"]->create( Fn::Property::"quatern_d", m_header->quatern_d );
//    m_properties["maingl"]->create( Fn::Property::"qoffset_x", m_header->qoffset_x );
//    m_properties["maingl"]->create( Fn::Property::"qoffset_y", m_header->qoffset_y );
//    m_properties["maingl"]->create( Fn::Property::"qoffset_z", m_header->qoffset_z );
//    m_properties["maingl"]->create( Fn::Property::"qfac", m_header->qfac );
//    m_properties["maingl"]->create( Fn::Property::qto_xyz", m_header->qto_xyz );
//    m_properties["maingl"]->create( Fn::Property::qto_ijk", m_header->qto_ijk );
//    m_properties["maingl"]->create( Fn::Property:: sto_xyz", m_header->sto_xyz );
//    m_properties["maingl"]->create( Fn::Property::"sto_ijk", m_header->sto_ijk );
//    m_properties["maingl"]->create( Fn::Property::"toffset", m_header->toffset );
//    m_properties["maingl"]->create( Fn::Property::"xyz_units", m_header->xyz_units );
//    m_properties["maingl"]->create( Fn::Property::"time_units", m_header->time_units );
//    m_properties["maingl"]->create( Fn::Property::"nifti_type", m_header->nifti_type );
//    m_properties["maingl"]->create( Fn::Property::"intent_code", m_header->intent_code );
//    m_properties["maingl"]->create( Fn::Property::"intent_p1", m_header->intent_p1 );
//    m_properties["maingl"]->create( Fn::Property::"intent_p2", m_header->intent_p2 );
//    m_properties["maingl"]->create( Fn::Property::"intent_p3", m_header->intent_p3 );
//    m_properties["maingl"]->create( Fn::Property::"intent_name", m_header->intent_name );
//    m_properties["maingl"]->create( Fn::Property::"descrip", m_header->descrip );
//    m_properties["maingl"]->create( Fn::Property::"aux_file", m_header->aux_file );
//    m_properties["maingl"]->create( Fn::Property::"swapsize", m_header->swapsize );
//    m_properties["maingl"]->create( Fn::Property::"byteorder", m_header->byteorder );

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

int DatasetNifti::getIdFromPos( float x, float y, float z )
{
    float dx = m_properties["maingl"]->get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties["maingl"]->get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties["maingl"]->get( Fn::Property::D_DZ ).toFloat();

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int px = x / dx;
    int py = y / dy;
    int pz = z / dz;

    px = qMax( 0, qMin( px, nx - 1) );
    py = qMax( 0, qMin( py, ny - 1) );
    pz = qMax( 0, qMin( pz, nz - 1) );

    return px + py * nx + pz * nx * ny;
}

int DatasetNifti::getId( int x, int y, int z )
{
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int px = qMax( 0, qMin( x, nx - 1) );
    int py = qMax( 0, qMin( y, ny - 1) );
    int pz = qMax( 0, qMin( z, nz - 1) );

    return px + py * nx + pz * nx * ny;
}
