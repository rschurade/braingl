/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASET3D_H_
#define DATASET3D_H_

#include <vector>

#include "nifti/nifti1_io.h"

#include "datasetnifti.h"

class Dataset3D : public DatasetNifti
{
public:
    Dataset3D( QString filename, std::vector<float> data );
    virtual ~Dataset3D();
};

#endif /* DATASET3D_H_ */
