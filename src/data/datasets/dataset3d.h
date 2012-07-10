/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASET3D_H_
#define DATASET3D_H_

#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "datasetnifti.h"

class Dataset3D : public DatasetNifti
{
public:
    Dataset3D( QString filename, QVector<QVector3D> data );
    virtual ~Dataset3D();

    void examineDataset();

private:
    void createTexture();

    QVector<QVector3D> m_data;
};

#endif /* DATASET3D_H_ */
