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

class ROI
{
public:
    ROI( QString name );
    virtual ~ROI();

    ROIPropertyGroup* properties();

protected:
    ROIPropertyGroup m_properties;
};

#endif /* ROI_H_ */
