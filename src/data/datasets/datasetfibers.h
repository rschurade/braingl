/*
 * datasetfibers.h
 *
 *  Created on: Dec 12, 2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETFIBERS_H_
#define DATASETFIBERS_H_

#include "dataset.h"

#include <QtCore/QString>

class QAbstractItemModel;
class FiberRenderer;

class DatasetFibers: public Dataset
{
public:
    DatasetFibers( QString filename, QVector< QVector< float > > fibs, int numPoints, int numLines );
    DatasetFibers( QVector< QVector< float > > fibs );
    DatasetFibers( QVector< QVector< float > > fibs, QVector< QVector< float > > extraData );
    virtual ~DatasetFibers();

    void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QAbstractItemModel* dataModel );
    QString getValueAsString( int x, int y, int z );

    QVector< QVector< float > > getFibs();
    QVector< QVector< float > > getSelectedFibs();

protected:
    void createTexture() {};

private:
    QVector< QVector< float > > m_fibs;
    QVector< QVector< float > > m_extraData;

    FiberRenderer* m_renderer;
};

#endif /* DATASETFIBERS_H_ */
