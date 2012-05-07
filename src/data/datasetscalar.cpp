/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( std::vector<float> data ) :
    Dataset(),
    m_data ( data )
{
}

DatasetScalar::~DatasetScalar()
{
}

void DatasetScalar::parseNiftiHeader( nifti_image* header )
{
    ndim = header->ndim;
    nx = header->nx;
    ny = header->ny;
    nz = header->nz;
    nt = header->nt;
    nu = header->nu;
    nv = header->nv;
    nw = header->nw;
    //dim = header->dim;
    nvox = header->nvox;
    nbyper = header->nbyper;
    datatype = header->datatype;

    dx = header->dx;
    dy = header->dy;
    dz = header->dz;
    dt = header->dt;
    du = header->du;
    dv = header->dv;
    dw = header->dw;
    //pixdim = header->pixdim;

    scl_slope = header->scl_slope;
    scl_inter = header->scl_inter;

    cal_min = header->cal_min;
    cal_max = header->cal_max;

    qform_code = header->qform_code;
    sform_code = header->sform_code;

    freq_dim = header->freq_dim;
    phase_dim = header->phase_dim;
    slice_dim = header->slice_dim;

    slice_code = header->slice_code;
    slice_start = header->slice_start;
    slice_end = header->slice_end;
    slice_duration = header->slice_duration;

    quatern_b = header->quatern_b;
    quatern_c = header->quatern_c;
    quatern_d = header->quatern_d;
    qoffset_x = header->qoffset_x;
    qoffset_y = header->qoffset_y;
    qoffset_z = header->qoffset_z;
    qfac = header->qfac;

    qto_xyz = header->qto_xyz;
    qto_ijk = header->qto_ijk;

    sto_xyz = header->sto_xyz;
    sto_ijk = header->sto_ijk;

    toffset = header->toffset;

    xyz_units = header->xyz_units;
    time_units = header->time_units;

    nifti_type = header->nifti_type;
    intent_code = header->intent_code;
    intent_p1 = header->intent_p1;
    intent_p2 = header->intent_p2;
    intent_p3 = header->intent_p3;
    //intent_name = header->intent_name;

    //descrip = header->descrip;
    //aux_file = header->aux_file;

    fname = header->fname;
    iname = header->iname;
    iname_offset = header->iname_offset;
    swapsize = header->swapsize;
    byteorder = header->byteorder;
}
