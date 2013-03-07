/*
 * roisphere.h
 *
 *  Created on: 06.03.2013
 *      Author: Ralph Schurade
 */

#ifndef ROISPHERE_H_
#define ROISPHERE_H_

#include "roi.h"

class SphereRenderer;

class ROISphere : public ROI
{
    Q_OBJECT

public:
    ROISphere( QAbstractItemModel* globals );
    virtual ~ROISphere();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvInverse );

    static int m_count;
private:
    SphereRenderer* m_renderer;

private slots:
    void propChanged();
    void globalChanged();
};

#endif /* ROIBALL_H_ */
