/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <QtCore/QDir>
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
    Dataset( QDir fileName, FN_DATASET_TYPE type );
    virtual ~Dataset();

    QDir getFilename();

    QString getName();
    void setName( QString name );

    FN_DATASET_TYPE getType();

    int getSize();
protected:
    QDir m_fileName;

    QString m_name;

    FN_DATASET_TYPE m_type;

    int m_size; // actual size in bytes that the data requires in memory
};

#endif /* DATASET_H_ */
