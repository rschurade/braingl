/*
 * selectionbox.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include "roi.h"

class SelectionBox : public ROI
{
public:
    SelectionBox( QString name, int x, int y, int z, float dx, float dy, float dz, bool neg );
    virtual ~SelectionBox();

};

#endif /* SELECTIONBOX_H_ */
