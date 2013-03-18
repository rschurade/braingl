/*
 * datasetnifti.h
 *
 *  Created on: May 9, 2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETNIFTI_H_
#define DATASETNIFTI_H_

#include "dataset.h"

#include "../../thirdparty/nifti/nifti1_io.h"

#include "../../thirdparty/newmat10/newmat.h"

class DatasetNifti: public Dataset
{
public:
    DatasetNifti( QString filename, Fn::DatasetType type, nifti_image* header );
    virtual ~DatasetNifti();

    nifti_image* getHeader();

    Matrix getQForm();
    Matrix getSForm();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix ) = 0;
    virtual QString getValueAsString( int x, int y, int z ) = 0;

    static QString getNiftiDataType( const int type );

protected:
    virtual void createTexture() = 0;
    virtual void examineDataset() = 0;
    virtual void flipX() = 0;
    void parseNiftiHeader();

    nifti_image* m_header;

    Matrix m_qform;
    Matrix m_sform;
};

#endif /* DATASETNIFTI_H_ */
