/*
 * datasetisoline.h
 *
 *  Created on: 08.05.2014
 *      Author: Ralph
 */

#ifndef DATASETISOLINE_H_
#define DATASETISOLINE_H_

#include "dataset.h"

class DatasetScalar;

class DatasetIsoline : public Dataset
{
    Q_OBJECT

public:
    DatasetIsoline( DatasetScalar* ds );
    virtual ~DatasetIsoline();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

private:
    std::vector<float> m_scalarField;

    bool m_dirty;

    GLuint vbo0;
    GLuint vbo1;

    int m_countLines;

    void initGeometry();

    std::vector<float>extractAnatomyAxial();
    std::vector<float>extractAnatomySagittal();
    std::vector<float>extractAnatomyCoronal();

    int getId( int x, int y, int z );

private slots:
    void isoValueChanged();

};

#endif /* DATASETISOLINE_H_ */
