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

    void set( FN_PROPERTY name, bool value, bool visible = false );
    void set( FN_PROPERTY name, int value, bool visible = false );
    void set( FN_PROPERTY name, float value, bool visible = false );
    void set( FN_PROPERTY name, QString value, bool visible = false );

    QVariant getVisible();

private:
    QHash<FN_PROPERTY, Property*> m_properties;
    QList<QVariant>m_visible;
};

#endif /* PROPERTYGROUP_H_ */
