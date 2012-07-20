/*
 * datasetscalar.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASETSCALAR_H_
#define DATASETSCALAR_H_

#include "datasetnifti.h"

class DatasetScalar : public DatasetNifti
{
public:
    DatasetScalar( QString filename, QVector<float> data );
    virtual ~DatasetScalar();

    void examineDataset(); //!< calls misc function to determine properties like min/max of the dataset

    QVector<float> getData();

private:
    QVector<float>m_data;

    void createTexture();
};

#endif /* DATASETSCALAR_H_ */
