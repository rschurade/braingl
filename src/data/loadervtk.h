/*
 * loadervtk.h
 *
 *  Created on: Apr 5, 2013
 *      Author: schurade
 */

#ifndef LOADERVTK_H_
#define LOADERVTK_H_

#include <QFile>
#include <QString>
#include <QStringList>
#include <QVector>

class LoaderVTK
{
public:
    LoaderVTK( QString fn );
    virtual ~LoaderVTK();

    bool load();
    QStringList getStatus();
    int getPrimitiveType();
    QVector<float> getPoints();
    QVector<int>getLines();
    QVector<int>getPolys();

    QVector<QVector<float> >getPointData();
    QVector<QString>getPointDataNames();

    QVector<unsigned char> getPointColors();

    int getNumPoints();
    int getNumLines();
    int getNumPolys();

private:
    bool exists();
    bool open();

    QString m_filename;
    QStringList m_status;
    int m_primitiveType; // 0 - undefined, 1 - POLYGONS, 2 - LINES
    int m_numPoints;
    int m_numLines;
    int m_numPolys;

    bool m_hasPointData;
    bool m_hasPrimitiveData;
    bool m_hasPointColors;
    bool m_hasPrimitiveColors;

    QVector<float>m_points;
    QVector<int>m_lines;
    QVector<int>m_polys;

    QVector<QVector<float> >m_pointData;
    QVector<QString>m_pointDataNames;

    QVector<QVector<float> >m_primitiveData;
    QVector<QString>m_primitiveDataNames;

    QVector<unsigned char>m_pointColors;
    QVector<float>m_primitiveColors;
};

#endif /* LOADERVTK_H_ */
