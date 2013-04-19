/*
 * dataset3d.h
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETTENSOR_H_
#define DATASETTENSOR_H_

#include "datasetnifti.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QVector>
#include <QVector3D>

class TensorRenderer;
class TensorRendererEV;

class DatasetTensor: public DatasetNifti
{
public:
    DatasetTensor( QString filename, QVector<Matrix> data, nifti_image* header );
    DatasetTensor( QString filename, QVector<QVector<float> > data, nifti_image* header );
    virtual ~DatasetTensor();

    QVector<Matrix>* getData();
    QVector<Matrix>* getLogData();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height );
    QString getValueAsString( int x, int y, int z );

private:
    void createTexture();
    void examineDataset();

    void createLogTensors();

    void flipX();

    QVector<Matrix> m_data;
    QVector<Matrix> m_logData;

    TensorRenderer* m_renderer;
    TensorRendererEV* m_rendererEV;

    bool m_renderGlpyhs;
};

#endif /* DATASETTensor_H_ */
