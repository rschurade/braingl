/*
 * loadernifti.h
 *
 *  Created on: Oct 11, 2013
 *      Author: schurade
 */

#ifndef LOADERNIFTI_H_
#define LOADERNIFTI_H_

#include "../data/datasets/dataset.h"

#include "../thirdparty/nifti/nifti1_io.h"

#include <QDir>

class LoaderNifti
{
public:
    LoaderNifti( QDir fileName );
    virtual ~LoaderNifti();

    bool load();
    bool askTimeSeries( int dim );

    std::vector<Dataset*> getDataset();
    Fn::DatasetType getDatasetType();

private:
    bool loadNiftiHeader( QString hdrPath );
    bool loadData( QString fileName );
    template<typename T> void copyData( T* inputData );

    bool loadNiftiScalar();
    bool loadNiftiVector3D();
    bool loadNiftiTensor();
    bool loadNiftiSH();
    bool loadNiftiBingham();
    bool loadNiftiFMRI();
    bool loadNiftiDWI( QString fileName );
    bool loadNiftiDWI_FNAV2( QString fileName );
    std::vector<float> loadBvals( QString fileName );
    std::vector<QVector3D> loadBvecs( QString fileName, std::vector<float> bvals );

    bool loadIsosurface();
    bool loadIsoline();

    bool isRadialogical();
    void flipX();

    QDir m_fileName;
    nifti_image* m_header;
    std::vector<float>m_data;
    Fn::DatasetType m_datasetType;
    std::vector<Dataset*> m_dataset;
    QList<QVariant>m_propStates;
};

#endif /* LOADERNIFTI_H_ */
