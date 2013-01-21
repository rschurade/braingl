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

    bool contains( FN_PROPERTY name );
    QVariant get( FN_PROPERTY name );

    void set( FN_PROPERTY name, bool value );
    void set( FN_PROPERTY name, int value );
    void set( FN_PROPERTY name, float value );
    void set( FN_PROPERTY name, QString value );

    QVariant getValue( FN_PROPERTY prop );

    QHash<FN_PROPERTY, Property*>getAll();

private:
    QHash<FN_PROPERTY, Property*> m_properties;
};

#endif /* PROPERTYGROUP_H_ */
