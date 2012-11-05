/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETQBALL_H_
#define DATASETQBALL_H_

#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "../../thirdparty/newmat10/newmat.h"

#include "datasetnifti.h"

class DatasetQBall : public DatasetNifti
{
public:
    DatasetQBall( QString filename, QVector<ColumnVector>* data, nifti_image* header );
    virtual ~DatasetQBall();

    QVector<ColumnVector>* getData();

private:
    void createTexture();
    void examineDataset();

    void flipX();

    QVector<ColumnVector>* m_data;
};

#endif /* DATASETQBALL_H_ */
