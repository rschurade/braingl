/*
 * dataset3d.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETSH_H_
#define DATASETSH_H_

#include "datasetnifti.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtCore/QVector>
#include <QtGui/QVector3D>

class QAbstractItemModel;
class SHRenderer;

class DatasetSH: public DatasetNifti
{
public:
    DatasetSH( QString filename, QVector<ColumnVector> data, nifti_image* header );
    virtual ~DatasetSH();

    QVector<ColumnVector>* getData();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel );
    QString getValueAsString( int x, int y, int z );

private:
    void createTexture();
    void examineDataset();

    void flipX();

    QVector<ColumnVector> m_data;

    SHRenderer* m_renderer;
};

#endif /* DATASETSH_H_ */
