/*
 * datasetfibers.h
 *
 *  Created on: Jan 16, 2014
 *      Author: schurade
 */

#ifndef DATASETFIBERS_H_
#define DATASETFIBERS_H_

#include "dataset.h"

#include "../../algos/fib.h"

#include <QDir>
#include <QList>
#include <QString>

class ColormapRenderer;
class FiberRenderer;
class FiberSelector;
class LoaderVTK;
class TubeRenderer;

class DatasetFibers : public Dataset
{
    Q_OBJECT

public:
    DatasetFibers( QDir filename, Fn::DatasetType type );
    DatasetFibers( QDir name, std::vector<Fib> fibs, QList<QString> dataNames );
    DatasetFibers( QDir filename, LoaderVTK* lv );
    virtual ~DatasetFibers();

    std::vector<Fib>* getFibs();
    std::vector<Fib> getSelectedFibs();

    unsigned int numVerts();
    unsigned int numLines();

    QList<QString> getDataNames();
    QString getSaveFilter();
    QString getDefaultSuffix();

    std::vector<float>getDataMins();
    std::vector<float>getDataMaxes();
    void setDataMins( std::vector<float> mins );
    void setDataMaxes( std::vector<float> maxes );

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

protected:
    void copyFromLoader( LoaderVTK* lv );
    void createProps();
    void calcBoundingBox();

    std::vector<Fib>m_fibs;
    QList<QString>m_dataNames;

    std::vector<float>* m_kdVerts;

    std::vector<float> m_dataMins;
    std::vector<float> m_dataMaxes;

    FiberRenderer* m_renderer;
    TubeRenderer* m_tubeRenderer;
    ColormapRenderer* m_colormapRenderer;

    FiberSelector* m_selector;
    QMatrix4x4 m_transform;
    unsigned int m_numPoints;
    unsigned int m_numLines;

    QMatrix4x4 m_userTransform;

private:

private slots:
    void colorChanged();
    void dataModeChanged();
    void transformChanged( QVariant value );
    void applyTransform();
};

#endif /* DATASETFIBERS_H_ */
