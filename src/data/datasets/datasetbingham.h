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
    DatasetBingham( QDir filename, std::vector<std::vector<float> > data, nifti_image* header );
    virtual ~DatasetBingham();

    std::vector<std::vector<float> >* getData();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

private:
    void createTexture();
    void examineDataset();

    std::vector<std::vector<float> > m_data;

    BinghamRenderer* m_renderer;
};

#endif /* DATASETBINGHAM_H_ */
