/*
 * dataset3d.h
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */

#ifndef DATASET3D_H_
#define DATASET3D_H_

#include "datasetnifti.h"

#include <QList>
#include <QVector>
#include <QVector3D>

class DatasetScalar;
class EVRenderer;
class StippleRenderer;

class Dataset3D: public DatasetNifti
{
    Q_OBJECT

public:
    Dataset3D( QDir filename, std::vector<QVector3D> data, nifti_image* header );
    virtual ~Dataset3D();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

    std::vector<QVector3D>* getData();

    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );

private:
    void examineDataset();
    void createTexture();

    std::vector<QVector3D> m_data;

    EVRenderer* m_renderer;
    StippleRenderer* m_stippleRenderer;

    QList<DatasetScalar*>m_scalarDSL;

public slots:
    void updateMaskSelect();

private slots:
    void switchRenderSticks();
    void switchRenderStipples();
    void scalingChanged();
    void probMaskChanged();
};

#endif /* DATASET3D_H_ */
