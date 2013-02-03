/*
 * roi.h
 *
 *  Created on: 03.02.2013
 *      Author: Ralph
 */

#ifndef ROI_H_
#define ROI_H_

#include "enums.h"
#include "properties/roipropertygroup.h"

class QAbstractItemModel;

class ROI
{
public:
    ROI( QString name, QAbstractItemModel* globals );
    virtual ~ROI();

    ROIPropertyGroup* properties();

protected:
    ROIPropertyGroup m_properties;
    QAbstractItemModel* m_globals;
};

#endif /* ROI_H_ */
