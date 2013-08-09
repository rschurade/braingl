/*
 * datasetfmri.h
 *
 *  Created on: Aug 9, 2013
 *      Author: schurade
 */

#ifndef DATASETFMRI_H_
#define DATASETFMRI_H_

#include "datasetnifti.h"

class DatasetFMRI: public DatasetNifti
{
    Q_OBJECT

public:
    DatasetFMRI( QDir filename, QVector<float> data, nifti_image* header );
    virtual ~DatasetFMRI();

    QVector<float>* getData();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );

private:
    QVector<float> m_data;

//    ColormapRenderer* m_colormapRenderer;

    void examineDataset(); //!< calls misc function to determine properties like min/max of the dataset
    void createTexture();

    void flipX();

    private slots:
        void selectTexture();
        void autoplay();

};

#endif /* DATASETFMRI_H_ */
