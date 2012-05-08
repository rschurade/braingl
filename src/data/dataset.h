/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <QtCore/QString>

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
    Dataset( QString fileName, FN_DATASET_TYPE type );
    virtual ~Dataset();

    QString getFilename();

    QString getShortFilename();

    FN_DATASET_TYPE getType();
private:
    QString m_fileName;
    FN_DATASET_TYPE m_type;
};

#endif /* DATASET_H_ */
