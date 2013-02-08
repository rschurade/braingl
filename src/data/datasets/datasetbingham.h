/*
 * datasetbingham.h
 *
 *  Created on: Nov 23, 2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETBINGHAM_H_
#define DATASETBINGHAM_H_

#include "datasetnifti.h"

#include <QtGui/QVector3D>
#include <QtCore/QVector>

class QAbstractItemModel;
class BinghamRenderer;

class DatasetBingham : public DatasetNifti
{
public:
    DatasetBingham( QString filename, QVector<QVector<float> > data, nifti_image* header );
    virtual ~DatasetBingham();

    QVector<QVector<float> >* getData();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel );
    QString getValueAsString( int x, int y, int z );

private:
    void createTexture();
    void examineDataset();

    void flipX();

    QVector<QVector<float> > m_data;

    BinghamRenderer* m_renderer;
};

#endif /* DATASETBINGHAM_H_ */
