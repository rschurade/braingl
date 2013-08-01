/*
 * datasetbingham.h
 *
 * Created on: Nov 23, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETBINGHAM_H_
#define DATASETBINGHAM_H_

#include "datasetnifti.h"

#include <QVector3D>
#include <QVector>

class BinghamRenderer;

class DatasetBingham : public DatasetNifti
{
public:
    DatasetBingham( QDir filename, QVector<QVector<float> > data, nifti_image* header );
    virtual ~DatasetBingham();

    QVector<QVector<float> >* getData();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

private:
    void createTexture();
    void examineDataset();

    void flipX();

    QVector<QVector<float> > m_data;

    BinghamRenderer* m_renderer;
};

#endif /* DATASETBINGHAM_H_ */
