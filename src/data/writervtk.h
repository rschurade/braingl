/*
 * writervtk.h
 *
 *  Created on: Sep 13, 2013
 *      Author: schurade
 */

#ifndef WRITERVTK_H_
#define WRITERVTK_H_

#include <QString>

class Dataset;
class TriangleMesh2;

class WriterVTK
{
public:
    WriterVTK( Dataset* dataset, QString fileName, QString filter = "" );
    virtual ~WriterVTK();

    bool save();

private:
    Dataset* m_dataset;
    QString m_fileName;
    QString m_filter;

    void saveFibs( QString filename, bool binary = true );

    void saveMesh( QString filename, TriangleMesh2* mesh, bool binary = true );
};

#endif /* WRITERVTK_H_ */
