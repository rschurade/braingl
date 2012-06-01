/*
 * datasetnifti.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETNIFTI_H_
#define DATASETNIFTI_H_

#include "../glew/include/glew.h"

#include "nifti/nifti1_io.h"

#include "dataset.h"

class DatasetNifti : public Dataset
{
public:
    DatasetNifti(  QString filename, FN_DATASET_TYPE type, void* data  );
    virtual ~DatasetNifti();

    void parseNiftiHeader( nifti_image* header );

    GLuint getTextureGLuint();

    virtual void examineDataset() = 0;

    nifti_image* getHeader();
    void* getData();

protected:
    virtual void createTexture() = 0;

    void* m_data;

    nifti_image* m_header;

    GLuint m_textureGLuint;
};

#endif /* DATASETNIFTI_H_ */
