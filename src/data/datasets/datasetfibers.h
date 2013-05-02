/*
 * datasetfibers.h
 *
 * Created on: Dec 12, 2012
 * @author Ralph Schurade
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
    Q_OBJECT

public:
    DatasetFibers( QDir filename, QVector< QVector< float > > fibs );
    DatasetFibers( QDir filename,
                     QVector< QVector< float > > fibs,
                     QVector<QVector< QVector< float > > >data,
                     QVector<QString> dataNames,
                     QVector< float > mins,
                     QVector<float> maxes );
    virtual ~DatasetFibers();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

    QVector< QVector< float > > getFibs();
    QVector< QVector< QVector< float > > >getData();
    QVector< QVector< float > > getSelectedFibs();

protected:
    void createTexture() {};

private:
    QVector< QVector< float > > m_fibs;
    QVector< QVector< QVector< float > > >m_data;
    QVector<QString>m_dataNames;

    QVector<float> m_dataMins;
    QVector<float> m_dataMaxes;

    FiberRenderer* m_renderer;
    TubeRenderer* m_tubeRenderer;
    FiberSelector* m_selector;

private slots:
    void colorChanged();
    void dataModeChanged();
};

#endif /* DATASETFIBERS_H_ */
