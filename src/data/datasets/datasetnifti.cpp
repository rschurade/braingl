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
        Dataset( filename, type ), m_header( header ), m_qform( 3, 3 ), m_sform( 3, 3 )
{
    parseNiftiHeader();
}

DatasetNifti::~DatasetNifti()
{
}

void DatasetNifti::parseNiftiHeader()
{
    m_properties["maingl"]->set( Fn::Property::DIM, m_header->ndim );
    m_properties["maingl"]->set( Fn::Property::NX, m_header->nx );
    m_properties["maingl"]->set( Fn::Property::NY, m_header->ny );
    m_properties["maingl"]->set( Fn::Property::NZ, m_header->nz );

//    m_properties["maingl"]->set( Fn::Property::NT, m_header->nt );
//    m_properties["maingl"]->set( Fn::Property::NU, m_header->nu );
//    m_properties["maingl"]->set( Fn::Property::NV, m_header->nv );
//    m_properties["maingl"]->set( Fn::Property::NW, m_header->nw );
//    m_properties["maingl"]->set("dim", m_header->dim );
//    m_properties["maingl"]->set( Fn::Property::NVOX, static_cast<int>( m_header->nvox ) );
//    m_properties["maingl"]->set( Fn::Property::NBYPER, m_header->nbyper );
    m_properties["maingl"]->set( Fn::Property::DATATYPE, m_header->datatype );
    m_properties["maingl"]->set( Fn::Property::DX, m_header->dx );
    m_properties["maingl"]->set( Fn::Property::DY, m_header->dy );
    m_properties["maingl"]->set( Fn::Property::DZ, m_header->dz );
//    m_properties["maingl"]->set( Fn::Property::DT, m_header->dt );
//    m_properties["maingl"]->set( Fn::Property::DU, m_header->du );
//    m_properties["maingl"]->set( Fn::Property::DV, m_header->dv );
//    m_properties["maingl"]->set( Fn::Property::DW, m_header->dw );
//    m_properties["maingl"]->set( Fn::Property::"pixdim", m_header->pixdim );
//    m_properties["maingl"]->set( Fn::Property::"scl_slope", m_header->scl_slope );
//    m_properties["maingl"]->set( Fn::Property::"scl_inter", m_header->scl_inter );
//    m_properties["maingl"]->set( Fn::Property::"cal_min", m_header->cal_min );
//    m_properties["maingl"]->set( Fn::Property::"cal_max", m_header->cal_max );
//    m_properties["maingl"]->set( Fn::Property::"qform_code", m_header->qform_code );
//    m_properties["maingl"]->set( Fn::Property::"sform_code", m_header->sform_code );
//    m_properties["maingl"]->set( Fn::Property::"freq_dim", m_header->freq_dim );
//    m_properties["maingl"]->set( Fn::Property::"phase_dim", m_header->phase_dim );
//    m_properties["maingl"]->set( Fn::Property::"slice_dim", m_header->slice_dim );
//    m_properties["maingl"]->set( Fn::Property::"slice_code", m_header->slice_code );
//    m_properties["maingl"]->set( Fn::Property::"slice_start", m_header->slice_start );
//    m_properties["maingl"]->set( Fn::Property::"slice_end", m_header->slice_end );
//    m_properties["maingl"]->set( Fn::Property::"slice_duration", m_header->slice_duration );
//    m_properties["maingl"]->set( Fn::Property::"quatern_b", m_header->quatern_b );
//    m_properties["maingl"]->set( Fn::Property::"quatern_c", m_header->quatern_c );
//    m_properties["maingl"]->set( Fn::Property::"quatern_d", m_header->quatern_d );
//    m_properties["maingl"]->set( Fn::Property::"qoffset_x", m_header->qoffset_x );
//    m_properties["maingl"]->set( Fn::Property::"qoffset_y", m_header->qoffset_y );
//    m_properties["maingl"]->set( Fn::Property::"qoffset_z", m_header->qoffset_z );
//    m_properties["maingl"]->set( Fn::Property::"qfac", m_header->qfac );
//    m_properties["maingl"]->set( Fn::Property::qto_xyz", m_header->qto_xyz );
//    m_properties["maingl"]->set( Fn::Property::qto_ijk", m_header->qto_ijk );
//    m_properties["maingl"]->set( Fn::Property:: sto_xyz", m_header->sto_xyz );
//    m_properties["maingl"]->set( Fn::Property::"sto_ijk", m_header->sto_ijk );
//    m_properties["maingl"]->set( Fn::Property::"toffset", m_header->toffset );
//    m_properties["maingl"]->set( Fn::Property::"xyz_units", m_header->xyz_units );
//    m_properties["maingl"]->set( Fn::Property::"time_units", m_header->time_units );
//    m_properties["maingl"]->set( Fn::Property::"nifti_type", m_header->nifti_type );
//    m_properties["maingl"]->set( Fn::Property::"intent_code", m_header->intent_code );
//    m_properties["maingl"]->set( Fn::Property::"intent_p1", m_header->intent_p1 );
//    m_properties["maingl"]->set( Fn::Property::"intent_p2", m_header->intent_p2 );
//    m_properties["maingl"]->set( Fn::Property::"intent_p3", m_header->intent_p3 );
//    m_properties["maingl"]->set( Fn::Property::"intent_name", m_header->intent_name );
//    m_properties["maingl"]->set( Fn::Property::"descrip", m_header->descrip );
//    m_properties["maingl"]->set( Fn::Property::"aux_file", m_header->aux_file );
//    m_properties["maingl"]->set( Fn::Property::"swapsize", m_header->swapsize );
//    m_properties["maingl"]->set( Fn::Property::"byteorder", m_header->byteorder );

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
    float dx = m_properties["maingl"]->get( Fn::Property::DX ).toFloat();
    float dy = m_properties["maingl"]->get( Fn::Property::DY ).toFloat();
    float dz = m_properties["maingl"]->get( Fn::Property::DZ ).toFloat();

    int nx = m_properties["maingl"]->get( Fn::Property::NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::NZ ).toInt();

    int px = x / dx;
    int py = y / dy;
    int pz = z / dz;

    px = qMax( 0, qMin( px, nx - 1) );
    py = qMax( 0, qMin( py, ny - 1) );
    pz = qMax( 0, qMin( pz, nz - 1) );

    return px + py * nx + pz * nx * ny;
}

QString DatasetNifti::getColormapShader( int num )
{
    if ( m_properties["maingl"]->get( Fn::Property::IS_ATLAS ).toBool() )
    {
        QString code( "" );
        code += "bool isBitSet( in float value, in float bitpos )\n";
        code += "{\n";
        code += "return ( abs( mod( floor( value / pow( 2.0, bitpos ) ), 2.0 ) - 1.0 ) ) < 0.001;\n";
        code += "}\n";

        code += "vec4 colormap" + QString::number( num ) + "( vec4 inColor, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax ) \n";
        code += "{ \n";
        code += "    float value = unpackFloat( inColor );\n";

        code += "float val = floor( value * 255.0 );\n";
        code += "float r = 0.0;\n";
        code += "float g = 0.0;\n";
        code += "float b = 0.0;\n";
        code += "float mult = 1.0;\n";

        code += "if( val == 0.0 )\n";
        code += "{\n";
        code += "return vec4( vec3( 0.0 ), 1.0 );\n";
        code += "}\n";

        code += "if( isBitSet( val, 0.0 ) )\n";
        code += "{\n";
        code += "b = 1.0;\n";
        code += "}\n";
        code += "if( isBitSet( val, 1.0 ) )\n";
        code += "{\n";
        code += "g = 1.0;\n";
        code += "}\n";
        code += "if( isBitSet( val, 2.0 ) )\n";
        code += "{\n";
        code += "r = 1.0;\n";
        code += "}\n";
        code += "if( isBitSet( val, 3.0 ) )\n";
        code += "{\n";
        code += "mult -= 0.15;\n";
        code += "if( r < 1.0 && g < 1.0 && b < 1.0 )\n";
        code += "{\n";
        code += "r = 1.0;\n";
        code += "g = 1.0;\n";
        code += "}\n";
        code += "}\n";
        code += "if( isBitSet( val, 4.0 ) )\n";
        code += "{\n";
        code += "mult -= 0.15;\n";
        code += "    if( r < 1.0 && g < 1.0 && b < 1.0 )\n";
        code += "{\n";
        code += "b = 1.0;\n";
        code += "g = 1.0;\n";
        code += "}\n";
        code += "}\n";
        code += "if( isBitSet( val, 5.0 ) )\n";
        code += "{\n";
        code += "mult -= 0.15;\n";
        code += "if( r < 1.0 && g < 1.0 && b < 1.0 )\n";
        code += "{\n";
        code += "r = 1.0;\n";
        code += "b = 1.0;\n";
        code += "}\n";
        code += "}\n";
        code += "if( isBitSet( val, 6.0 ) )\n";
        code += "{\n";
        code += "mult -= 0.15;\n";
        code += "if( r < 1.0 && g < 1.0 && b < 1.0 )\n";
        code += "{\n";
        code += "g = 1.0;\n";
        code += "}\n";
        code += "}\n";
        code += "if( isBitSet( val, 7.0 ) )\n";
        code += "{\n";
        code += "mult -= 0.15;\n";
        code += "if( r < 1.0 && g < 1.0 && b < 1.0 )\n";
        code += "{\n";
        code += "r = 1.0;\n";
        code += "}\n";
        code += "}\n";

        code += "r *= mult;\n";
        code += "g *= mult;\n";
        code += "b *= mult;\n";

        code += "clamp( r, 0.0, 1.0 );\n";
        code += "clamp( g, 0.0, 1.0 );\n";
        code += "clamp( b, 0.0, 1.0 );\n";

        code += "return vec4( r, g, b, 1.0 );\n";
        code += "}\n";
        return code;

    }
    else
    {
        int colormap = m_properties["maingl"]->get( Fn::Property::COLORMAP ).toInt();

        QString code( "" );

        code += "vec4 colormap" + QString::number( num ) + "( vec4 inColor, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax ) \n";
        code += "{ \n";
        code += "    float value = unpackFloat( inColor );\n";
        code += "    vec3 color = vec3(0.0); \n";
        code += "    if ( value < lowerThreshold ) \n";
        code += "    { \n";
        code += "        return vec4( 0.0 ); \n";
        code += "    } \n";
        code += "    if ( value > upperThreshold ) \n";
        code += "    { \n";
        code += "        return vec4( 0.0 ); \n";
        code += "    } \n";
        code += "    value = ( value - selectedMin ) / ( selectedMax - selectedMin ); \n";
        code += "\n";
        code += ColormapFunctions::get( colormap ).getCode();
        code += "\n";
        code += "    return vec4( color, 0.0 ); \n";
        code += "} \n";

        return code;
    }
}
