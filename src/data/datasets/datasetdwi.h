/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETDWI_H_
#define DATASETDWI_H_

#include <QtGui/QVector3D>
#include <QtCore/QVector>

#include "../../thirdparty/newmat10/newmat.h"

#include "datasetnifti.h"

class DatasetDWI : public DatasetNifti
{
public:
    DatasetDWI( QString filename, QVector<ColumnVector>* data, QVector<float> b0Data, QVector<int>bvals, QVector<QVector3D>bvecs );
    virtual ~DatasetDWI();

    void examineDataset();

    QVector<ColumnVector>* getData();

    QVector<float> getB0Data();

    QVector<int> getBvals();

    QVector<QVector3D> getBvecs();

private:
    void createTexture();

    QVector<ColumnVector>* m_data;
    QVector<float> m_b0Data;
    QVector<int>m_bvals;
    QVector<QVector3D>m_bvecs;

};

#endif /* DATASET3D_H_ */
