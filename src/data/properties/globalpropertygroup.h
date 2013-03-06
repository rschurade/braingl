/*
 * globalpropertygroup.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
 */

#ifndef GLOBALPROPERTYGROUP_H_
#define GLOBALPROPERTYGROUP_H_

#include "property.h"
#include "propertybool.h"
#include "propertycolor.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertypath.h"
#include "propertystring.h"

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class GlobalPropertyGroup : public QObject
{
    Q_OBJECT

public:
    GlobalPropertyGroup();
    virtual ~GlobalPropertyGroup();

    bool contains( Fn::Global name ) const;
    QVariant get( Fn::Global name ) const;

    bool set( Fn::Global name, bool value, bool visible = false );
    bool set( Fn::Global name, int value, bool visible = false );
    bool set( Fn::Global name, int value, int min, int max, bool visible = false );
    bool set( Fn::Global name, float value, bool visible = false );
    bool set( Fn::Global name, float value, float min, float max, bool visible = false );
    bool set( Fn::Global name, QString value, bool visible = false );
    bool set( Fn::Global name, const char* value, bool visible = false );
    bool set( Fn::Global name, QVariant value );
    bool set( Fn::Global name, QColor value, bool visible = false );
    bool set( Fn::Global name, QDir value, bool visible = false );

    bool setMin( Fn::Global name,  float value );
    bool setMax( Fn::Global name,  float value );
    bool setMin( Fn::Global name,  int value );
    bool setMax( Fn::Global name,  int value );

    int size() const;

    QList<Fn::Global> getVisible();
    QWidget* getWidget( Fn::Global name );

private:
    QHash<int, Property*> m_properties;
    QList<Fn::Global>m_visible;

private slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
};

#endif /* PROPERTYGROUP_H_ */
