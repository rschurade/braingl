/*
 * Loader.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <QtCore/QDir>

#include "../thirdparty/nifti/nifti1_io.h"

#include "datasets/dataset.h"

class Loader
{
public:
    Loader( QDir fileName );
    virtual ~Loader();

    bool load();

    Dataset* getDataset();

    bool succes();
private:
    FN_DATASET_TYPE determineType();

    void* loadNifti( QString fileName );
    void loadDWI( QString fileName );

    nifti_image* m_header;
    QDir m_fileName;
    FN_DATASET_TYPE m_datasetType;

    Dataset* m_dataset;

    bool m_success;

};

#endif /* LOADER_H_ */
