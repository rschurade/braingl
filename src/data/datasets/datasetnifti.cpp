/*
 * datasetnifti.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "datasetnifti.h"

#include "../../gui/gl/colormapfunctions.h"

#include <QDebug>
#include <QWidget>

DatasetNifti::DatasetNifti( QDir filename, Fn::DatasetType type, nifti_image* header ) :
    Dataset( filename, type ),
    m_header( header )
{
    parseNiftiHeader();
}

DatasetNifti::~DatasetNifti()
{
}

void DatasetNifti::parseNiftiHeader()
{
    m_properties["maingl"]->createInt( Fn::Property::D_DIM, m_header->dim[4] );
    m_properties["maingl"]->createInt( Fn::Property::D_NX, m_header->nx );
    m_properties["maingl"]->createInt( Fn::Property::D_NY, m_header->ny );
    m_properties["maingl"]->createInt( Fn::Property::D_NZ, m_header->nz );

//    m_properties["maingl"]->create( Fn::Property::NT, m_header->nt );
//    m_properties["maingl"]->create( Fn::Property::NU, m_header->nu );
//    m_properties["maingl"]->create( Fn::Property::NV, m_header->nv );
//    m_properties["maingl"]->create( Fn::Property::NW, m_header->nw );
//    m_properties["maingl"]->create("dim", m_header->dim );
//    m_properties["maingl"]->create( Fn::Property::NVOX, static_cast<int>( m_header->nvox ) );
//    m_properties["maingl"]->create( Fn::Property::NBYPER, m_header->nbyper );
    m_properties["maingl"]->createInt( Fn::Property::D_DATATYPE, m_header->datatype );
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

    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j )
        {
            m_qform( i, j ) = m_header->qto_xyz.m[i][j];
        }
    }

    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j )
        {
            m_sform( i, j ) = m_header->sto_xyz.m[i][j];
        }
    }


//    m_qform( 0, 3 ) = m_header->qoffset_x;
//    m_qform( 1, 3 ) = m_header->qoffset_y;
//    m_qform( 2, 3 ) = m_header->qoffset_z;

    m_header->descrip[0] = 'f';
    m_header->descrip[1] = 'n';
    m_header->descrip[2] = 'a';
    m_header->descrip[3] = 'v';
    m_header->descrip[4] = '2';
    m_header->descrip[5] = 0;

    QMatrix4x4 sform;
    QMatrix4x4 qform;

    //m_properties["maingl"]->create( Fn::Property::D_USE_TRANSFORM, { "none", "sform", "qform" }, 0, "transform" );
    m_properties["maingl"]->createMatrix( Fn::Property::D_S_FORM, m_sform, "transform" );
    m_properties["maingl"]->createMatrix( Fn::Property::D_Q_FORM, m_qform, "transform" );
    m_properties["maingl"]->getWidget( Fn::Property::D_S_FORM )->setEnabled( false );
    m_properties["maingl"]->getWidget( Fn::Property::D_Q_FORM )->setEnabled( false );

    m_properties["maingl"]->createFloat( Fn::Property::D_DX, m_header->dx, 0.01, 10.0, "transform" );
    m_properties["maingl"]->createFloat( Fn::Property::D_DY, m_header->dy, 0.01, 10.0, "transform" );
    m_properties["maingl"]->createFloat( Fn::Property::D_DZ, m_header->dz, 0.01, 10.0, "transform" );
    if ( m_header->sform_code > 0 )
    {
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_X, m_sform( 0, 3 ), -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Y, m_sform( 1, 3 ), -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Z, m_sform( 2, 3 ), -250, 250, "transform" );
    }
    else if ( m_header->sform_code > 0 )
    {
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_X, m_qform( 0, 3), -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Y, m_qform( 1, 3), -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Z, m_qform( 2, 3), -250, 250, "transform" );
    }
    else
    {
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_X, 0, -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Y, 0, -250, 250, "transform" );
        m_properties["maingl"]->createFloat( Fn::Property::D_ADJUST_Z, 0, -250, 250, "transform" );
    }
}

nifti_image* DatasetNifti::getHeader()
{
    return m_header;
}

QMatrix4x4 DatasetNifti::getQForm()
{
    return m_qform;
}

QMatrix4x4 DatasetNifti::getSForm()
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

int DatasetNifti::getIdFromPos( QVector3D pos )
{
    return getIdFromPos( pos.x(), pos.y(), pos.z() );
}

int DatasetNifti::getIdFromPos( float x, float y, float z )
{
    float dx = m_properties["maingl"]->get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties["maingl"]->get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties["maingl"]->get( Fn::Property::D_DZ ).toFloat();

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int px = ( x + dx / 2 ) / dx;
    int py = ( y + dy / 2 ) / dy;
    int pz = ( z + dz / 2 ) / dz;


    px = qMax( 0, qMin( px, nx - 1 ) );
    py = qMax( 0, qMin( py, ny - 1 ) );
    pz = qMax( 0, qMin( pz, nz - 1 ) );

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

void DatasetNifti::getXYZ( int id, int &x, int &y, int &z )
{
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();

    x = id % nx;
    int tempY = id % ( nx * ny );
    y = tempY / nx;
    z = id / ( nx * ny );
}

QList<int> DatasetNifti::getNeighbourhood3x3( int x, int y, int z )
{
    QList<int> out;
    out.push_back( getId( x - 1, y - 1, z - 1 ) );
    out.push_back( getId( x - 1, y    , z - 1 ) );
    out.push_back( getId( x - 1, y + 1, z - 1 ) );
    out.push_back( getId( x    , y - 1, z - 1 ) );
    out.push_back( getId( x    , y    , z - 1 ) );
    out.push_back( getId( x    , y + 1, z - 1 ) );
    out.push_back( getId( x + 1, y - 1, z - 1 ) );
    out.push_back( getId( x + 1, y    , z - 1 ) );
    out.push_back( getId( x + 1, y + 1, z - 1 ) );

    out.push_back( getId( x - 1, y - 1, z ) );
    out.push_back( getId( x - 1, y    , z ) );
    out.push_back( getId( x - 1, y + 1, z ) );
    out.push_back( getId( x    , y - 1, z ) );
    out.push_back( getId( x    , y    , z ) );
    out.push_back( getId( x    , y + 1, z ) );
    out.push_back( getId( x + 1, y - 1, z ) );
    out.push_back( getId( x + 1, y    , z ) );
    out.push_back( getId( x + 1, y + 1, z ) );

    out.push_back( getId( x - 1, y - 1, z + 1 ) );
    out.push_back( getId( x - 1, y    , z + 1 ) );
    out.push_back( getId( x - 1, y + 1, z + 1 ) );
    out.push_back( getId( x    , y - 1, z + 1 ) );
    out.push_back( getId( x    , y    , z + 1 ) );
    out.push_back( getId( x    , y + 1, z + 1 ) );
    out.push_back( getId( x + 1, y - 1, z + 1 ) );
    out.push_back( getId( x + 1, y    , z + 1 ) );
    out.push_back( getId( x + 1, y + 1, z + 1 ) );

    return out;
}

QList<int> DatasetNifti::getNeighbourhoodXxX( int x, int y, int z, int dist )
{
    QList<int>out;

    for(int zz = z - dist; zz <= z + dist; ++zz )
    {
        for(int yy = y - dist; yy <= y + dist; ++yy )
        {
            for(int xx = x - dist; xx <= x + dist; ++xx )
            {
                out.push_back( getId( xx, yy, zz ) );
            }
        }
    }
    return out;
}

QString DatasetNifti::getSaveFilter()
{
    return QString( "niftii (*.nii *.nii.gz);;all files (*.*)" );
}

QString DatasetNifti::getDefaultSuffix()
{
    return QString( "nii.gz" );
}

QColor DatasetNifti::getColorAtPos( float x, float y, float z )
{
    return QColor( 0, 0, 0 ,0 );
}
