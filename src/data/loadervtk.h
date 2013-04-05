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
    QString getStatus();
    int getPrimitiveType();
    QVector<float> getPoints();
    QVector<int>getPrimitives();

    int getNumPoints();
    int getNumPrimitives();

private:
    bool exists();
    bool open();
    bool loadHeader();

    bool loadAscii();
    bool loadPointsAscii();
    bool loadPrimitivesAscii();

    bool loadBinary();

    QString readLine( QDataStream& in );

    QString m_filename;
    QString m_status;
    int m_type; // 0 - undefined, 1 - ASCII, 2 - BINARY
    int m_primitiveType; // 0 - undefined, 1 - POLYGONS, 2 - LINES

    int m_numPoints;
    int m_numPrimitives;
    int m_primitiveSize;

    QFile* m_file;
    QStringList m_header;

    QVector<float>m_points;
    QVector<int>m_primitives;
};

#endif /* LOADERVTK_H_ */
