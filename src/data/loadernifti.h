/*
 * loadernifti.h
 *
 *  Created on: Oct 11, 2013
 *      Author: schurade
 */

#ifndef LOADERNIFTI_H_
#define LOADERNIFTI_H_

#include "datasets/dataset.h"

#include "../thirdparty/nifti/nifti1_io.h"

#include <QDir>

class LoaderNifti
{
public:
    LoaderNifti( QDir fileName );
    virtual ~LoaderNifti();

    bool load();

    QVector<Dataset*> getDataset();
    Fn::DatasetType getDatasetType();

private:
    bool loadNiftiHeader( QString hdrPath );

    bool loadNiftiScalar( QString fileName );
    bool loadNiftiVector3D( QString fileName );
    bool loadNiftiTensor( QString fileName );
    bool loadNiftiSH( QString fileName );
    bool loadNiftiBingham( QString fileName );
    bool loadNiftiFMRI( QString fileName );
    bool loadNiftiDWI( QString fileName );
    bool loadNiftiDWI_FNAV2( QString fileName );
    std::vector<float> loadBvals( QString fileName );
    QVector<QVector3D> loadBvecs( QString fileName, std::vector<float> bvals );

    bool isRadialogical();
    void flipX();

    template<typename T> void copyScalar( T* inputData );
    template<typename T> void copyVector( T* inputData );

    QDir m_fileName;
    nifti_image* m_header;
    std::vector<float>m_scalarData;
    QVector<QVector3D>m_vectorData;
    Fn::DatasetType m_datasetType;
    QVector<Dataset*> m_dataset;
};

#endif /* LOADERNIFTI_H_ */
