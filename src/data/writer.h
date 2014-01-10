/*
 * writer.h
 *
 * Created on: Jun 1, 2012
 * @author Ralph Schurade
 */

#ifndef WRITER_H_
#define WRITER_H_

#include "../thirdparty/nifti/nifti1_io.h"

#include <QFileInfo>
#include <QString>

class Dataset;
class TriangleMesh2;

class Writer
{
public:
    Writer( Dataset* dataset, QFileInfo fileName, QString file = "" );
    virtual ~Writer();

    bool save();

private:
    Dataset* m_dataset;
    QFileInfo m_fileName;
    QString m_filter;

    nifti_image* createHeader( int dim );

    void setDescrip( nifti_image* hdr, QString descrip );

    void saveRGB();
    void save1D();
    void saveROI();
    void saveConnexels();
    void saveOBJ();
    void saveVRML();
};

#endif /* WRITER_H_ */
