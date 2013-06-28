/*
 * datasetscalar.h
 *
 * Created on: May 4, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETSCALAR_H_
#define DATASETSCALAR_H_

#include "datasetnifti.h"

class ColormapRenderer;

class DatasetScalar: public DatasetNifti
{
public:
    DatasetScalar( QDir filename, QVector<float> data, nifti_image* header );
    virtual ~DatasetScalar();

    QVector<float>* getData();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

    void mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers );

private:
    QVector<float> m_data;

    ColormapRenderer* m_colormapRenderer;

    void examineDataset(); //!< calls misc function to determine properties like min/max of the dataset
    void createTexture();

    void flipX();
};

#endif /* DATASETSCALAR_H_ */
