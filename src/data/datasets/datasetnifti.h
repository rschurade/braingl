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
    DatasetNifti( QString filename, FN_DATASET_TYPE type );
    virtual ~DatasetNifti();

    void parseNiftiHeader( nifti_image* header );

    GLuint getTextureGLuint();

    virtual void examineDataset() = 0;

    nifti_image* getHeader();

    Matrix getQForm();
    Matrix getSForm();

protected:
    virtual void createTexture() = 0;

    nifti_image* m_header;

    GLuint m_textureGLuint;

    Matrix m_qform;
    Matrix m_sform;
};

#endif /* DATASETNIFTI_H_ */
