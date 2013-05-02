/*
 * dataset3d.h
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */

#ifndef DATASET3D_H_
#define DATASET3D_H_

#include "datasetnifti.h"

#include <QVector>
#include <QVector3D>

class EVRenderer;

class Dataset3D: public DatasetNifti
{
public:
    Dataset3D( QDir filename, QVector<QVector3D> data, nifti_image* header );
    virtual ~Dataset3D();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );
    QString getValueAsString( int x, int y, int z );

    QVector<QVector3D>* getData();

    void mousePick( int pickId, QVector3D pos );

private:
    void examineDataset();
    void createTexture();

    void flipX();

    QVector<QVector3D> m_data;

    EVRenderer* m_renderer;
};

#endif /* DATASET3D_H_ */
