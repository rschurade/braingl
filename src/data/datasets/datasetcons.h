/*
 * datasetcons.h
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#ifndef DATASETCONS_H_
#define DATASETCONS_H_

#include "../../algos/connections.h"
#include "../../gui/gl/vectorglyphrenderer.h"

#include "dataset.h"

#include <QString>

class DatasetCons: public Dataset
{
public:
    DatasetCons( QString filename );
    DatasetCons( Connections* cons );
    virtual ~DatasetCons();
    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString();

    Connections* getCons();
    float* getVectors();

private:
    Connections* cons;
    VectorGlyphRenderer* m_vrenderer;
    float* vectorArray;
    int consNumber;
};

#endif /* DATASETCONS_H_ */
