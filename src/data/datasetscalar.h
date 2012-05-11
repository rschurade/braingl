/*
 * datasetscalar.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASETSCALAR_H_
#define DATASETSCALAR_H_

#include "nifti/nifti1_io.h"

#include "datasetnifti.h"

class DatasetScalar : public DatasetNifti
{
public:
    DatasetScalar( QString filename, void* data );
    virtual ~DatasetScalar();

private:
    void createTexture();

    float m_min;
    float m_max;
};

#endif /* DATASETSCALAR_H_ */
