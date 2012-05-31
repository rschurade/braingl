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
#include <QtCore/QHash>
#include <QtCore/QVariant>

enum FN_DATASET_TYPE
{
    FNDT_UNKNOWN,
    FNDT_NIFTI_SCALAR,
    FNDT_NIFTI_VECTOR,
    FNDT_NIFTI_4D,
    FNDT_NIFTI2_SCALAR,
    FNDT_NIFTI2_VECTOR,
    FNDT_NIFTI2_4D
};

class Dataset
{
public:
    Dataset( QDir fileName, FN_DATASET_TYPE type );
    virtual ~Dataset();

    QVariant getProperty( QString name );
    void setProperty( QString name, QVariant value );

protected:
    QHash< QString, QVariant >m_properties;
};

#endif /* DATASET_H_ */
