/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#include "dataset3d.h"

Dataset3D::Dataset3D( QString filename, std::vector<float> data )  :
    DatasetNifti( filename, FNDT_NIFTI_VECTOR, data )
{
}

Dataset3D::~Dataset3D()
{
}

