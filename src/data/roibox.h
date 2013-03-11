/*
 * roibox.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph Schurade
 */

#ifndef ROIBOX_H_
#define ROIBOX_H_

#include "roi.h"

class ROIBox : public ROI
{
    Q_OBJECT

public:
    ROIBox( QAbstractItemModel* globals );
    virtual ~ROIBox();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );

    static int m_count;
private:

private slots:
    void propChanged();
    void globalChanged();
};

#endif /* SELECTIONBOX_H_ */
