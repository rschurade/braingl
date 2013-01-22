/*
 * propertygroup.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#ifndef PROPERTYGROUP_H_
#define PROPERTYGROUP_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include "property.h"
#include "propertybool.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertystring.h"

class PropertyGroup : public QObject
{
    Q_OBJECT

public:
    PropertyGroup();
    virtual ~PropertyGroup();

    bool contains( FN_PROPERTY name );
    QVariant get( FN_PROPERTY name ) const;

    void set( FN_PROPERTY name, bool value, bool visible = false );
    void set( FN_PROPERTY name, int value, bool visible = false );
    void set( FN_PROPERTY name, int value, int min, int max, bool visible = false );
    void set( FN_PROPERTY name, float value, bool visible = false );
    void set( FN_PROPERTY name, float value, float min, float max, bool visible = false );
    void set( FN_PROPERTY name, QString value, bool visible = false );
    void set( FN_PROPERTY name, QVariant value );

    QList<FN_PROPERTY> getVisible();
    QWidget* getWidget( FN_PROPERTY name );

private:
    QHash<FN_PROPERTY, Property*> m_properties;
    QList<FN_PROPERTY>m_visible;

private slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
};

#endif /* PROPERTYGROUP_H_ */
