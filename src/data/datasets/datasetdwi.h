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

class QAbstractItemModel;

class DatasetDWI: public DatasetNifti
{
public:
    DatasetDWI( QString filename, QVector<ColumnVector> data, QVector<float> b0Data, QVector<float> bvals, QVector<QVector3D> bvecs, nifti_image* header );
    virtual ~DatasetDWI();

    QVector<ColumnVector>* getData();

    QVector<float>* getB0Data();

    QVector<float> getBvals();

    QVector<QVector3D> getBvecs();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model );
    QString getValueAsString( int x, int y, int z );

private:
    void examineDataset();
    void createTexture();

    void flipX();

    QVector<ColumnVector> m_data;
    QVector<float> m_b0Data;
    QVector<float> m_bvals;
    QVector<QVector3D> m_bvecs;

};

#endif /* DATASET3D_H_ */
