/*
 * Loader.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <QtCore/QString>

#include <nifti1_io.h>

#include "dataset.h"

class Loader
{
public:
    Loader( QString filename );
    virtual ~Loader();

    Dataset* load();

private:
    FN_DATASET_TYPE determineType();

    void loadNifti();

    nifti_image* m_header;
    QString m_filename;
    FN_DATASET_TYPE m_datasetType;

};

#endif /* LOADER_H_ */
