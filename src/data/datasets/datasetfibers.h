/*
 * datasetfibers.h
 *
 *  Created on: Dec 12, 2012
 *      Author: schurade
 */

#ifndef DATASETFIBERS_H_
#define DATASETFIBERS_H_

#include <QtCore/QString>

#include "dataset.h"

class FiberRenderer;

class DatasetFibers: public Dataset
{
public:
    DatasetFibers( QVector< QVector< float > > fibs, int numPoints, int numLines );
    virtual ~DatasetFibers();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore );
    QString getValueAsString( int x, int y, int z );

    QVector< QVector< float > > getFibs();

protected:
    void createTexture() {};

private:
    QVector< QVector< float > > m_fibs;

    FiberRenderer* m_renderer;
};

#endif /* DATASETFIBERS_H_ */
