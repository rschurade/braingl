/*
 * propertygroup.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#ifndef PROPERTYGROUP_H_
#define PROPERTYGROUP_H_

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "property.h"
#include "propertybool.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertystring.h"

class PropertyGroup
{
public:
    PropertyGroup();
    virtual ~PropertyGroup();

    void addProperty( PropertyBool prop );
    void addProperty( PropertyInt prop );
    void addProperty( PropertyFloat prop );
    void addProperty( PropertyString prop );

    QVariant getValue( FN_PROPERTY prop );

private:
    QHash<FN_PROPERTY, Property*> m_properties;
};

#endif /* PROPERTYGROUP_H_ */
