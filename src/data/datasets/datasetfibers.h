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
class LoaderVTK;

class DatasetFibers: public Dataset
{
    Q_OBJECT

public:
    DatasetFibers( QDir filename, Fn::DatasetType type );
    DatasetFibers( QDir filename, QVector< std::vector<float> > fibs );
    DatasetFibers( QDir filename,
                     QVector< std::vector<float> > fibs,
                     QVector<QVector< std::vector<float> > >data,
                     QVector<QString> dataNames,
                     std::vector<float> mins,
                     std::vector<float> maxes );

    DatasetFibers( QDir filename, LoaderVTK* lv );

    virtual ~DatasetFibers();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    QString getValueAsString( int x, int y, int z );

    QVector< std::vector<float> > getFibs();
    QVector< QVector< std::vector<float> > >getData();
    QVector< std::vector<float> > getData( int id );
    QVector< std::vector<float> > getSelectedFibs();
    QColor getCustomColor( int fiber );
    QVector<QString>getDataNames();
    QVector< QVector< std::vector<float> > > getSelectedData();
    std::vector<float> getDataMins();
    std::vector<float> getDataMaxes();

    QString getSaveFilter();
    QString getDefaultSuffix();

protected:
    void createTexture() {};
    void createProps();

    void copyFromLoader( LoaderVTK* lv );
    QVector< std::vector<float> > m_fibs;
    QVector< QVector< std::vector<float> > >m_data;
    QVector<QString>m_dataNames;
    QVector<QColor>m_customColors;

    std::vector<float> m_dataMins;
    std::vector<float> m_dataMaxes;

    FiberRenderer* m_renderer;
    TubeRenderer* m_tubeRenderer;
    FiberSelector* m_selector;
    QMatrix4x4 m_transform;
    unsigned int m_numPoints;
    unsigned int m_numLines;

    bool m_morphable;
    float m_morphValue;
    void makeMorphable();
    void morph(float i);
    QVector< std::vector<float> > m_orig_fibs;
    QVector< std::vector<float> > m_straight_fibs;

private slots:
    void colorChanged();
    void dataModeChanged();
    void autoplay();
    void transformChanged( QVariant value );
    void applyTransform();
};

#endif /* DATASETFIBERS_H_ */
