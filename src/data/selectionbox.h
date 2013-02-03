/*
 * selectionbox.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include "enums.h"
#include "properties/propertygroup.h"

class SelectionBox
{
public:
    SelectionBox( QString name );
    virtual ~SelectionBox();

    PropertyGroup* properties();

private:
    PropertyGroup m_properties;
};

#endif /* SELECTIONBOX_H_ */
