/*
 * dataset3d.h
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETDWI_H_
#define DATASETDWI_H_

#include "datasetnifti.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QVector3D>
#include <QVector>

class DatasetDWI: public DatasetNifti
{
    Q_OBJECT

public:
    DatasetDWI( QString filename, QVector<ColumnVector> data, QVector<float> b0Data, QVector<float> bvals, QVector<QVector3D> bvecs, nifti_image* header );
    virtual ~DatasetDWI();

    QVector<ColumnVector>* getData();

    QVector<float>* getB0Data();

    QVector<float> getBvals();

    QVector<QVector3D> getBvecs();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height );
    QString getValueAsString( int x, int y, int z );

private:
    void examineDataset();
    void createTexture();

    void flipX();

    QVector<ColumnVector> m_data;
    QVector<float> m_b0Data;
    QVector<float> m_bvals;
    QVector<QVector3D> m_bvecs;

private slots:
    void selectTexture();

};

#endif /* DATASET3D_H_ */
