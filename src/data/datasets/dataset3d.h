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

class EVRenderer;

class Dataset3D : public DatasetNifti
{
public:
    Dataset3D( QString filename, QVector<QVector3D> data, nifti_image* header );
    virtual ~Dataset3D();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore );
    QString getValueAsString( int x, int y, int z );

    QVector<QVector3D>* getData();

private:
    void examineDataset();
    void createTexture();

    void flipX();

    QVector<QVector3D> m_data;

    EVRenderer* m_renderer;
};

#endif /* DATASET3D_H_ */
