/*
 * roibox.h
 *
 * Created on: 02.02.2013
 * @author Ralph Schurade
 */

#ifndef ROIBOX_H_
#define ROIBOX_H_

#include "roi.h"

class ROIBox : public ROI
{
    Q_OBJECT

public:
    ROIBox();
    virtual ~ROIBox();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );

    static int m_count;
private:

private slots:
    void propChanged();
    void globalChanged();
};

#endif /* SELECTIONBOX_H_ */
