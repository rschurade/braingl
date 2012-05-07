/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <string>

enum FN_DATASET_TYPE
{
    FNDT_UNKNOWN,
    FNDT_NIFTI_SCALAR,
    FNDT_NIFTI_VECTOR,
    FNDT_NIFTI_4D
};

class Dataset
{
public:
    Dataset( std::string filename );
    virtual ~Dataset();

    std::string getFilename();
private:
    std::string m_filename;
};

#endif /* DATASET_H_ */
