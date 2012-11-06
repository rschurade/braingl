/*
 * datasetnifti.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETNIFTI_H_
#define DATASETNIFTI_H_

#include "../../thirdparty/glew/include/glew.h"

#include "../../thirdparty/nifti/nifti1_io.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "dataset.h"

class DatasetNifti : public Dataset
{
public:
    DatasetNifti( QString filename, FN_DATASET_TYPE type, nifti_image* header );
    virtual ~DatasetNifti();

    GLuint getTextureGLuint();

    nifti_image* getHeader();

    Matrix getQForm();
    Matrix getSForm();

    virtual void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore ) = 0;

protected:
    virtual void createTexture() = 0;
    virtual void examineDataset() = 0;
    virtual void flipX() = 0;
    void parseNiftiHeader();

    nifti_image* m_header;

    GLuint m_textureGLuint;

    Matrix m_qform;
    Matrix m_sform;
};

#endif /* DATASETNIFTI_H_ */
