/*
 * datasetscalar.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#include "datasetscalar.h"

DatasetScalar::DatasetScalar( QString filename, std::vector<float> data ) :
    DatasetNifti( filename, FNDT_NIFTI_SCALAR, data )
{
}

DatasetScalar::~DatasetScalar()
{
}

