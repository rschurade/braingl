/*
 * roiwriter.h
 *
 * Created on: Jul 20, 2014
 * @author Ralph Schurade
 */

#ifndef ROIWRITER_H_
#define ROIWRITER_H_

#include "../thirdparty/nifti/nifti1_io.h"

#include <QFileInfo>
#include <QString>

class ROIArea;

class RoiWriter
{
public:
    RoiWriter( ROIArea* roi, QFileInfo fileName );
    virtual ~RoiWriter();

    bool save();

    nifti_image* createHeader( int dim );

private:
    ROIArea* m_roi;
    QFileInfo m_fileName;

    void setDescrip( nifti_image* hdr, QString descrip );
};

#endif /* ROIWRITER_H_ */
