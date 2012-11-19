/*
 * Loader.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <QtCore/QDir>
#include <QtCore/QVector>

#include "../thirdparty/nifti/nifti1_io.h"

#include "datasets/dataset.h"

class Loader
{
public:
    Loader( QDir fileName );
    virtual ~Loader();

    bool load();

    Dataset* getDataset( int id = 0 );
    int getNumDatasets();

    bool succes();
private:
    FN_DATASET_TYPE determineType();

    bool loadNifti();
    bool loadNiftiHeader( QString hdrPath );

    bool loadNiftiScalar( QString fileName );
    bool loadNiftiVector3D( QString fileName );
    bool loadNiftiTensor( QString fileName );
    bool loadNiftiQBall( QString fileName );
    bool loadNiftiDWI( QString fileName );
    bool loadNiftiDWI_FNAV2( QString fileName );
    QVector<float> loadBvals( QString fileName );
    QVector<QVector3D>loadBvecs( QString fileName, QVector<float> bvals );



    bool loadMesh();
    bool loadMeshBinary();

    nifti_image* m_header;
    QDir m_fileName;

    FN_DATASET_TYPE m_datasetType;
    QVector<Dataset*> m_dataset;

    bool m_success;

};

#endif /* LOADER_H_ */
