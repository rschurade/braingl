/*
 * Loader.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <vector>

#include <QtCore/QString>

#include "nifti/nifti1_io.h"

#include "dataset.h"

class Loader
{
public:
    Loader( QString fileName );
    virtual ~Loader();

    bool load();

    Dataset* getDataset();

    bool succes();
private:
    FN_DATASET_TYPE determineType();

    void* loadNifti();

    nifti_image* m_header;
    QString m_fileName;
    FN_DATASET_TYPE m_datasetType;

    Dataset* m_dataset;

    bool m_success;

};

#endif /* LOADER_H_ */
