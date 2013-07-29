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

private:

private slots:
    void propChanged();
    void globalChanged();
    void dxChanged( QVariant value );
};

#endif /* SELECTIONBOX_H_ */
