/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETTENSOR_H_
#define DATASETTENSOR_H_

#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "../../thirdparty/newmat10/newmat.h"

#include "datasetnifti.h"

class DatasetTensor : public DatasetNifti
{
public:
    DatasetTensor( QString filename, QVector<Matrix>* data, nifti_image* header );
    virtual ~DatasetTensor();

    QVector<Matrix>* getData();

private:
    void createTexture();
    void examineDataset();

    void flipX();

    QVector<Matrix>* m_data;
};

#endif /* DATASETTensor_H_ */
