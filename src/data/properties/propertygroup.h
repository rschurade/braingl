/*
 * propertygroup.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
 */

#ifndef PROPERTYGROUP_H_
#define PROPERTYGROUP_H_

#include "property.h"
#include "propertybool.h"
#include "propertycolor.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertystring.h"

#include <QColor>
#include <QObject>
#include <QString>
#include <QVariant>

class PropertyGroup : public QObject
{
    Q_OBJECT

public:
    PropertyGroup();
    virtual ~PropertyGroup();

    bool contains( Fn::Property name ) const;
    QVariant get( Fn::Property name ) const;

    bool set( Fn::Property name, bool value, bool visible = false );
    bool set( Fn::Property name, int value, bool visible = false );
    bool set( Fn::Property name, int value, int min, int max, bool visible = false );
    bool set( Fn::Property name, float value, bool visible = false );
    bool set( Fn::Property name, float value, float min, float max, bool visible = false );
    bool set( Fn::Property name, QString value, bool visible = false );
    bool set( Fn::Property name, const char* value, bool visible = false );
    bool set( Fn::Property name, QVariant value );
    bool set( Fn::Property name, QColor value, bool visible = false );

    int size() const;

    QList<Fn::Property> getVisible();
    QWidget* getWidget( Fn::Property name );

    Property* getProperty( Fn::Property name );

private:
    QHash<int, Property*> m_properties;
    QList<Fn::Property>m_visible;

public slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
};

#endif /* PROPERTYGROUP_H_ */
