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
    std::vector<float>* getPoints();
    std::vector<int>getLines();
    std::vector<int>getPolys();

    std::vector<std::vector<float> >getPointData();
    QList<QString>getPointDataNames();

    std::vector<unsigned char> getPointColors();
    std::vector<unsigned char> getPrimitiveColors();

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

    std::vector<float>* m_points;
    std::vector<int>m_lines;
    std::vector<int>m_polys;

    std::vector<std::vector<float> >m_pointData;
    QList<QString>m_pointDataNames;

    std::vector<std::vector<float> >m_primitiveData;
    std::vector<QString>m_primitiveDataNames;

    std::vector<unsigned char>m_pointColors;
    std::vector<unsigned char>m_primitiveColors;
};

#endif /* LOADERVTK_H_ */
