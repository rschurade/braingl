/*
 * datasetfibers.h
 *
 *  Created on: Dec 12, 2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETFIBERS_H_
#define DATASETFIBERS_H_

#include "dataset.h"

#include <QString>

class FiberRenderer;
class TubeRenderer;
class FiberSelector;

class DatasetFibers: public Dataset
{
public:
    DatasetFibers( QString filename, QVector< QVector< float > > fibs );
    DatasetFibers( QString filename, QVector< QVector< float > > fibs, QVector< QVector< float > > extras );
    virtual ~DatasetFibers();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );
    QString getValueAsString( int x, int y, int z );

    QVector< QVector< float > > getFibs();
    QVector< QVector< float > > getExtra();
    QVector< QVector< float > > getSelectedFibs();

protected:
    void createTexture() {};

private:
    QVector< QVector< float > > m_fibs;
    QVector< QVector< float > > m_extraData;

    FiberRenderer* m_renderer;
    TubeRenderer* m_tubeRenderer;
    FiberSelector* m_selector;
};

#endif /* DATASETFIBERS_H_ */
