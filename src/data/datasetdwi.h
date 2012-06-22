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

#include <newmat/newmat.h>

#include "nifti/nifti1_io.h"

#include "datasetnifti.h"

class DatasetDWI : public DatasetNifti
{
public:
    DatasetDWI( QString filename, QVector<NEWMAT::ColumnVector>* data, void* b0Data, QVector<int>bvals, QVector<QVector3D>bvecs );
    virtual ~DatasetDWI();

    void examineDataset();

    QVector<NEWMAT::ColumnVector>* getDataVector();

    void* getB0Data();

    QVector<int> getBvals();

    QVector<QVector3D> getBvecs();

private:
    void createTexture();

    QVector<NEWMAT::ColumnVector>* m_dataVector;
    void* m_b0Data;
    QVector<int>m_bvals;
    QVector<QVector3D>m_bvecs;

};

#endif /* DATASET3D_H_ */
